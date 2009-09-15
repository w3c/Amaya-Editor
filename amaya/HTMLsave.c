/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya saving functions.
 *
 * Authors: I. Vatton, L. Carcone, D. Veillard, J. Kahan
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug information when saving */

#include "wx/wx.h"
#include "wx/dir.h"
#include "wx/filename.h"  // for wxFileName

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "parser.h"
#include "css.h"
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#include "XML.h"
#include "zlib.h"
#include "message_wx.h"
#include "wxdialogapi_f.h"
#include "archives.h"
#include "registry_wx.h"
#include "email.h"
#include "wxdialog/SendByMailDlgWX.h"

#ifdef ANNOTATIONS
#include "Annot.h"
#include "annotlib.h"
#include "ANNOTevent_f.h"
#include "ANNOTtools_f.h"
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
#include "bookmarks.h"
#include "BMevent_f.h"
#endif /* BOOKMARKS */
#include "MENUconf.h"

#include "css_f.h"
#include "EDITORactions_f.h"
#include "fetchXMLname_f.h"
#include "HTMLhistory_f.h"
#include "UIcss_f.h"
#include "styleparser_f.h"
#include "HTMLform_f.h"

#ifdef TEMPLATES
#include "Template.h"
#include "templates.h"
#include "templateDeclarations_f.h"
#include "templates_f.h"
#endif /* TEMPLATES */

typedef struct _AttSearch
{
  int   att;
  int   type;
} AttSearch;

#include "wxdialogapi_f.h"
#include "appdialogue_wx.h"

#define StdDefaultName "Overview.html"
static char         tempSavedObject[MAX_LENGTH];
static ThotBool     TextFormat;
static ThotBool     Saving_lock = FALSE;
static ThotBool     Saving_All_lock = FALSE;
/* list attributes checked for updating URLs */
static AttSearch    URL_attr_tab[] = {
  {HTML_ATTR_HREF_, XHTML_TYPE},
  {HTML_ATTR_codebase, XHTML_TYPE},
  {HTML_ATTR_script_src, XHTML_TYPE},
  {HTML_ATTR_Script_URL, XHTML_TYPE},
  {HTML_ATTR_SRC, XHTML_TYPE},
  {HTML_ATTR_data, XHTML_TYPE},
  {HTML_ATTR_background_, XHTML_TYPE},
  {HTML_ATTR_Style_, XHTML_TYPE},
  {HTML_ATTR_cite, XHTML_TYPE},
  {XLink_ATTR_href_, XLINK_TYPE},
  {MathML_ATTR_style_, MATH_TYPE},
  {SVG_ATTR_style_, SVG_TYPE},
  {SVG_ATTR_xlink_href, SVG_TYPE}
};

/* list of attributes checked for updating images */
static AttSearch    SRC_attr_tab[] = {
  {HTML_ATTR_SRC, XHTML_TYPE},
  {HTML_ATTR_data, XHTML_TYPE},
  {HTML_ATTR_background_, XHTML_TYPE},
  {HTML_ATTR_Style_, XHTML_TYPE},
  {MathML_ATTR_style_, MATH_TYPE},
  {SVG_ATTR_style_, SVG_TYPE},
  {SVG_ATTR_xlink_href, SVG_TYPE}
};

#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"

/* the structure used for storing the context of the 
   SaveWikiFile_callback function */
typedef struct WIKI_context {
  char *localfile;
  char *output;
} WIKI_context;

// use HTMLform buffer to store post parameters
extern char  *FormBuf;    /* temporary buffer used to build the query string */
extern int    FormLength;  /* size of the temporary buffer */
extern int    FormBufIndex; /* gives the index of the last char + 1 added to
                              the buffer (only used in AddBufferWithEos) */

/*----------------------------------------------------------------------
  CheckValidProfile
  If it's a HTML document, do not export elements (SVG/MathML) not allowed by
  the document profile.
  ----------------------------------------------------------------------*/
ThotBool CheckValidProfile (NotifyElement *event)
{
  int       profile;

  profile = TtaGetDocumentProfile(event->document);
  if (DocumentTypes[event->document] != docHTML)
    return FALSE;  /* let Thot perform normal operation */
  else if (profile == L_Other || profile == L_Xhtml11 || profile == L_Transitional)
    return FALSE;  /* let Thot perform normal operation */
  else
    return TRUE;
}

/*----------------------------------------------------------------------
  CheckGenerator
  ----------------------------------------------------------------------*/
ThotBool CheckGenerator (NotifyElement *event)
{
  AttributeType      attrType;
  Attribute          attr;
  char               buff[MAX_LENGTH];
  char              *ptr;
  int                length;
  ElementType        elType;

  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr != 0)
    {
      length = MAX_LENGTH - 1;
      TtaGiveTextAttributeValue (attr, buff, &length);
      if (!strcasecmp (buff, "generator"))
        {
          /* is it Amaya generator ? */
          attrType.AttrTypeNum = HTML_ATTR_meta_content;
          attr = TtaGetAttribute (event->element, attrType);
          if (attr != 0)
            {
              length = MAX_LENGTH - 1;
              TtaGiveTextAttributeValue (attr, buff, &length);
              ptr = strstr (buff, "amaya");
              if (ptr == NULL)
                ptr = strstr (buff, "Amaya");
              if (ptr == NULL)
                /* it's not a pure Amaya document -> remove the meta element */
                return TRUE;
            }
        }
    }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  CheckUSEMAP
  Usemap starts with # except for XHTML 1.1 document.
  ----------------------------------------------------------------------*/
ThotBool CheckUSEMAP (NotifyAttribute *event)
{
  int                  length, profile, doc;
  char                *url;

  length = TtaGetTextAttributeLength (event->attribute) + 3;
  url = (char *)TtaGetMemory (length);
  doc = event->document;
  TtaGiveTextAttributeValue (event->attribute, &url[1], &length);
  profile = TtaGetDocumentProfile (doc);
  if (profile == L_Xhtml11 && url[1] == '#')
    // remove the #
    TtaSetAttributeText (event->attribute, &url[2], event->element, doc);
  else if (profile != L_Xhtml11 && url[1] != '#')
    {
      // add a #
      url[0] = '#';
      TtaSetAttributeText (event->attribute, &url[0], event->element, doc);
    }
  TtaFreeMemory (url);
  /* the document has a DocType */
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  CheckValidEntity
  An Entity name attribute is about to be saved. If the document doesn't
  have a DocType, replace the entity name by an entity value.
  ----------------------------------------------------------------------*/
ThotBool CheckValidEntity (NotifyAttribute *event)
{
  AttributeType     attrType;
  Attribute         attr;
  CHAR_T            c[3];
  Language	        lang;
  char              mbc[20], *s;
  int               length;
  ThotBool          withDocType, useMath;

  HasADoctype (event->document, &withDocType, &useMath);
  if (withDocType)
    /* the document has a DocType */
    return FALSE;  /* let Thot perform normal operation */

  attrType = event->attributeType;
  s = TtaGetSSchemaName (attrType.AttrSSchema);
  if (strcmp (s, "HTML") == 0)
    attrType.AttrTypeNum = HTML_ATTR_EntityName;
  else if (strcmp (s, "MathML") == 0)
    attrType.AttrTypeNum = MathML_ATTR_EntityName;
  else
    return FALSE;  /* let Thot perform normal operation */

  /* this function applies only to MathML elements */
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
    /* there is an EntityName attribute and the document has no DocType */
    {
      /* replace the entity name by its value */
      length = 2;
      TtaGiveBufferContent (event->element, c, length, &lang);
      memset (mbc, 0, 20);
      mbc[0] = '&';
      mbc[1] = '#';
      mbc[2] = 'x';
      sprintf (&mbc[3], "%x", (int)c[0]);
      strcat (mbc, ";");
      TtaSetAttributeText (attr, mbc, event->element, event->document);
    }
  return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  CheckValidID
  A NAME attribute is about to be saved. If the output format is XML
  and the current element does not have an ID attribute, check if
  the value of the NAME attribute is a valid XML ID and if not,
  generate an ID attribute with a valid value.
  ----------------------------------------------------------------------*/
ThotBool CheckValidID (NotifyAttribute *event)
{
  AttributeType     attrType;
  Attribute         attr;
  char             *value;
  int               length, i;

  if (!SaveAsXML)
    /* we are not saving the document in XML */
    return FALSE;  /* let Thot perform normal operation */

  attrType = event->attributeType;
  attrType.AttrTypeNum = HTML_ATTR_ID;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    /* this element does not have an ID attribute */
    {
      /* get the value of the NAME attribute */
      length = TtaGetTextAttributeLength (event->attribute);
      length+= 10;
      value = (char *)TtaGetMemory (length);
      length--;
      TtaGiveTextAttributeValue (event->attribute, &value[1], &length);
      if (value[1] >= '0' && value[1] <= '9')
        /* the value of the NAME attribute starts with a digit */
        {
          /* insert an underscore at the beginning and create an ID attribute
             with that value */
          value[0] = '_';
          length++;
          /* check that this value is not already used by another ID attribute
             in the document and add a number at the end if it's the case */
          i = 0;
          while (SearchNAMEattribute (event->document, value, NULL, NULL))
            /* this value is already used in the document */
            {
              i++;
              sprintf (&value[length], "%d", i);
            }
          /* Create the ID attr. */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (event->element, attr, event->document);
          TtaSetAttributeText (attr, value, event->element, event->document);
        }
      TtaFreeMemory (value);
    }
  return FALSE;  /* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  LoadADocResource loads a resource file
  ----------------------------------------------------------------------*/
static void LoadADocResource (Document doc, char *url, char *localfile)
{
  int                 toparse;

  if (localfile[0] != EOS)
    {
      UpdateTransfer (doc);
      toparse = GetObjectWWW (doc, 0, url, NULL, localfile,
                              AMAYA_SYNC | AMAYA_LOAD_CSS, NULL, NULL,
                              NULL, NULL, NO, NULL);
      ResetStop (doc);
    }
}

/*----------------------------------------------------------------------
  UpdateDocResource searches ".css" and ".js" urls within the sString
  and makes these urls relative to the newpath.
  oldpath = the old path
  newpath = the new path
  relpath = new relative path for files
  el refers the element that link this resource.
  saveResources is TRUE if resources except CSS (scripts) must be saved.
  isLink is TRUE if the string is a link attribute.
  fullCopy is TRUE if local resources must be copied.
  A new url is obtained by concatenation of relpath and the file name.
  Returns NULL or a new allocated sString.
  ----------------------------------------------------------------------*/
char *UpdateDocResource (Document doc, char *oldpath, char *newpath,
                         char *relpath, char *sString, Element el,
                         ThotBool saveResources, ThotBool isLink, ThotBool fullCopy)
{
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  ElementType         elType;
  LoadedImageDesc    *desc;
  char               *b, *e, *newString, *oldptr;
  char                old_url[MAX_LENGTH];
  char                new_url[MAX_LENGTH];
  char                filename[MAX_LENGTH];
  char                oldname[MAX_LENGTH];
  char                newname[MAX_LENGTH];
  char               *tempdocument = NULL;
  int                 len, newlen;
  ThotBool            src_is_local, dst_is_local;
  ThotBool            toSave = saveResources, isCSS = FALSE;

  newString = NULL;
  if (saveResources && !IsResourceName (sString))
    // don't consider a html document as a resource
    return newString;

  // look for a css
  b = strstr (sString, ".css");
  if (b && saveResources)
    isCSS = TRUE; // check if this CSS must be saved
  else
    b = sString;

  if (b)
    {
      // look for the beginning and the end of the url
      e = b;
      while (*e != '"' && *e != '\'' && *e != EOS)
        e++;
      while (*b != '"' && *b != '\'' && b != sString)
        b--;
      if (*b == '"' || *b == '\'')
        b++;
      len = (int)(e - b);
      strncpy (oldname, b, len);
      oldname[len] = EOS;
      // get the old full URL and the name of the file
      NormalizeURL (oldname, 0, old_url, filename, oldpath);
      if ((fullCopy || IsW3Path (newpath)) && !toSave &&
          !IsW3Path (oldpath) && !IsW3Path (old_url))
        toSave = TRUE;
      if (isCSS)
        {
          pInfo = NULL;
          css = SearchCSS (doc, old_url, el, &pInfo);
          if (css)
            // will be managed later
            return newString;
          else
            // save an alternate css not in the CSS table
            toSave = TRUE;
        }

      /* build the new full image name */
      if (toSave)
        {
          // the resource location will change
          if (relpath)
            strcpy (newname, relpath);
          else
            newname[0] = EOS;
          strcat (newname, filename);
        }
      else
        {
          // recompute the relative URL
          if (!strcmp (old_url, oldpath))
            // the link points to the document itself
            TtaExtractName (newpath, old_url, newname);
          else
            {
              tempdocument = MakeRelativeURL (old_url, newpath);
              strcpy (newname, tempdocument);
              TtaFreeMemory (tempdocument);
              tempdocument = NULL;
            }
        }
      newlen = strlen (newname);
      if (newlen != len || strcmp (oldname, newname))
        {
#ifdef AMAYA_DEBUG
fprintf(stderr, "Changed URL from %s to %s\n", oldname, newname);
#endif
          /* generate the new string */
          if (newlen > len)
            {
              // a memory allocation is necessary
              if (newString)
                oldptr = newString;
              else
                oldptr = sString;
              
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
              oldptr = &b[newlen];
              if (newlen < len)
                // reduce the size of the full string
                strcpy (oldptr, e);
              // note that a change is done
              if (newString == NULL)
                newString = sString;
            }
        }

      if (toSave)
        {
          // don't save picture units (it's done for the enclosing image)
          elType = TtaGetElementType (el);
          toSave = (elType.ElTypeNum != HTML_EL_PICTURE_UNIT);
        }
      if (toSave && newname[0] != EOS && old_url[0] != EOS)
        {
          // get the new full URL of the file
          NormalizeURL (newname, 0, new_url, filename, newpath);
          src_is_local = !IsW3Path (old_url);
          dst_is_local = !IsW3Path (new_url);
#ifdef AMAYA_DEBUG
fprintf(stderr, "Move file: from %s to %s\n", old_url, new_url);
#endif
          if (!src_is_local)
            {
              // load the file first
              tempdocument = GetLocalPath (0, old_url);
              LoadADocResource (doc, old_url, tempdocument);
              strcpy (old_url, tempdocument);
            }
          if (old_url[0] != EOS && TtaFileExist (old_url))
            {
              if (saveResources)
                {
              if (dst_is_local)
                /* copy the file to the new location */
                TtaFileCopy (old_url, new_url);
              else
                AddLocalResource (old_url, filename, new_url, doc,
                                  &desc, &LoadedResources, TRUE);
              if (!src_is_local)
                // remove the temporay file
                TtaFileUnlink (tempdocument);
                }
            }
          TtaFreeMemory (tempdocument);
        }
    }
  return newString;
}

/*----------------------------------------------------------------------
  SetRelativeURLs updates all URLs of the current document according to
  the new path. If possible, new URLs will be relative to this new path.
  resbase points to the directory where resource (CSS + scripts) are stored.
  When savedImages is TRUE Images src are not updated,
  When savedResources is TRUE CSS links are not updated as they are saved.
  fullCopy is TRUE if local resources must be copied.
  saveAs is TRUE if changes concern a saveAs command.
  ----------------------------------------------------------------------*/
void SetRelativeURLs (Document doc, char *newpath, char *resbase,
                      ThotBool savedImages, ThotBool savedResources,
                      ThotBool fullCopy, ThotBool saveAs)
{
  SSchema             XHTMLSSchema, MathSSchema, SVGSSchema, XLinkSSchema;
#ifdef TEMPLATES
  SSchema             TemplateSSchema = TtaGetSSchema ("Template", doc);
#endif /* TEMPLATES */
  Element             el, root, content, next = NULL;
  ElementType         elType, contentType;
  ElementType         searchedType1, searchedType2;
  ElementType         searchedType3, searchedType4, searchedType5;
  Attribute           attr;
  AttributeType       attrType;
  Language            lang;
  char                oldpath[MAX_LENGTH];
  char               *newString, *orgString;
  int                 index, max;
  int                 buflen;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SetRelativeURLs\n");
#endif
  XHTMLSSchema = TtaGetSSchema ("HTML", doc);
  MathSSchema = TtaGetSSchema ("MathML", doc);
  SVGSSchema = TtaGetSSchema ("SVG", doc);
  XLinkSSchema = TtaGetSSchema ("XLink", doc);
  root = TtaGetMainRoot (doc);
  strcpy (oldpath, DocumentURLs[doc]);

  /* Handle style elements */
  elType = TtaGetElementType (root);
  searchedType1.ElSSchema = elType.ElSSchema;
  searchedType2.ElSSchema = elType.ElSSchema;
  searchedType3.ElSSchema = elType.ElSSchema;
  searchedType4.ElSSchema = elType.ElSSchema;
  searchedType5.ElSSchema = elType.ElSSchema;
  searchedType1.ElTypeNum = 0;
  searchedType2.ElTypeNum = 0;
  searchedType3.ElTypeNum = 0;
  searchedType4.ElTypeNum = 0;
  searchedType5.ElTypeNum = 0;
  if (elType.ElSSchema != XLinkSSchema)
    {
      if (elType.ElSSchema == XHTMLSSchema)
        {
          searchedType1.ElTypeNum = HTML_EL_STYLE_;
          searchedType2.ElTypeNum = HTML_EL_PI_line;
          searchedType3.ElTypeNum = HTML_EL_PI_line;
          searchedType4.ElTypeNum = HTML_EL_PI_line;
          searchedType5.ElTypeNum = HTML_EL_PI_line;
        }
      else if (elType.ElSSchema == SVGSSchema)
        {
          searchedType1.ElTypeNum = SVG_EL_style__;
          searchedType2.ElTypeNum = SVG_EL_XMLPI_line;
          searchedType3.ElTypeNum = SVG_EL_XMLPI_line;
          searchedType4.ElTypeNum = SVG_EL_XMLPI_line;
          searchedType5.ElTypeNum = SVG_EL_XMLPI_line;
        }
      else if (elType.ElSSchema == MathSSchema)
        {
          searchedType1.ElTypeNum = MathML_EL_XMLPI_line;
          searchedType2.ElTypeNum = MathML_EL_XMLPI_line;
          searchedType3.ElTypeNum = MathML_EL_XMLPI_line;
          searchedType4.ElTypeNum = MathML_EL_XMLPI_line;
          searchedType5.ElTypeNum = MathML_EL_XMLPI_line;
        }
      else
        {
          searchedType1.ElTypeNum = XML_EL_xmlpi_line;
          searchedType2.ElTypeNum = XML_EL_xmlpi_line;
          searchedType3.ElTypeNum = XML_EL_xmlpi_line;
          searchedType4.ElTypeNum = XML_EL_xmlpi_line;
          searchedType5.ElTypeNum = XML_EL_xmlpi_line;
        }
      el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                        searchedType3, searchedType4,
                                        searchedType5, SearchInTree, root);
    }
  else
    el = NULL;

  while (el)
    {
      /* this is a style element */
      elType = TtaGetElementType (el);
      content = TtaGetFirstChild (el);
      if (content)
        {
          contentType = TtaGetElementType (content);
#ifdef TEMPLATES
          if ((contentType.ElTypeNum == Template_EL_useEl ||
               contentType.ElTypeNum == Template_EL_useSimple) &&
              contentType.ElSSchema == TemplateSSchema)
            {
              // Go inside the template use element
              content = TtaGetFirstChild (content);
              contentType = TtaGetElementType (content);
            }
#endif /* TEMPLATES */
        }
      if (content)
        {
          if (contentType.ElTypeNum == HTML_EL_TEXT_UNIT)
            // the style content is not inside a comment
            next = content;
          else
            {
              // manange text units inside the comment
              contentType.ElTypeNum = HTML_EL_TEXT_UNIT;
              next = TtaSearchTypedElementInTree (contentType, SearchForward, next,
                                                  content);
            }

          while (next)
            {
              buflen = TtaGetTextLength (content) + 1;
              orgString = (char *)TtaGetMemory (buflen);
              TtaGiveTextContent (content, (unsigned char *)orgString, &buflen, &lang);
              if ((elType.ElSSchema == XHTMLSSchema ||
                   elType.ElSSchema == SVGSSchema) &&
                  elType.ElTypeNum == searchedType1.ElTypeNum)
                // Manage the style string with no image save and no import update
                newString = UpdateCSSURLs (doc, oldpath, newpath, NULL, orgString,
                                        FALSE, FALSE);
              else if (strstr (orgString, ".xtd") == NULL &&
                       strstr (orgString, ".css") == NULL)
                // skip that PI
                newString = NULL;
              else
                // Update the XML PI content
                newString = UpdateDocResource (doc, oldpath, newpath,
                                               resbase, orgString,
                                               el, savedResources, TRUE, fullCopy);

              if (newString)
                {
                  /* register the modification to be able to undo it */
                  TtaRegisterElementReplace (next, doc);
                  TtaSetTextContent (next, (unsigned char *)newString, lang, doc);
                  TtaFreeMemory (newString);
                  newString = NULL;
                }
              else
                // no change
                TtaFreeMemory (orgString);

                
              if (next == content)
                next = NULL;
              else
                // next text unit
                next = TtaSearchTypedElementInTree (contentType, SearchForward, next,
                                                    next);
            }
        }

      // look for another PI or style element
      el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                        searchedType3, searchedType4,
                                        searchedType5, SearchForward, el);
    }

  /* Manage URLs and SRCs attributes */
  max = sizeof (URL_attr_tab) / sizeof (AttSearch);
  for (index = 0; index < max; index++)
    {
      /* search all elements having this attribute */
      attrType.AttrTypeNum = URL_attr_tab[index].att;
      switch (URL_attr_tab[index].type)
        {
        case XHTML_TYPE:
          attrType.AttrSSchema = XHTMLSSchema;
          break;
        case MATH_TYPE:
          attrType.AttrSSchema = MathSSchema;
          break;
        case SVG_TYPE:
          attrType.AttrSSchema = SVGSSchema;
          break;
        case XLINK_TYPE:
          attrType.AttrSSchema = XLinkSSchema;
          break;
        default:
          attrType.AttrSSchema = NULL;
        }
      if (attrType.AttrSSchema)
        TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
      else
        el = NULL;
      while (el && attr)
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum != HTML_EL_BASE || elType.ElSSchema != XHTMLSSchema)
            {
              buflen = TtaGetTextAttributeLength (attr) + 1;
              orgString = (char *)TtaGetMemory (buflen);
              TtaGiveTextAttributeValue (attr, orgString, &buflen);
              if ((attrType.AttrTypeNum == HTML_ATTR_Style_ &&
                   attrType.AttrSSchema == XHTMLSSchema) ||
                  (attrType.AttrTypeNum == MathML_ATTR_style_ &&
                   attrType.AttrSSchema == MathSSchema) ||
                  (attrType.AttrTypeNum == SVG_ATTR_style_  &&
                   attrType.AttrSSchema == SVGSSchema))
                {
                  /* update URLs in the style attribute */
                  // No save images and no new local import
                  newString = UpdateCSSURLs (doc, oldpath, newpath, NULL,
                                      orgString, FALSE, FALSE);
                  if (newString)
                    {
                      /* register the modification to be able to undo it */
                      TtaRegisterAttributeReplace (attr, el, doc);
                      TtaSetAttributeText (attr, newString, el, doc);
                      TtaFreeMemory (newString);
                    }
                  else
                    // no change
                    TtaFreeMemory (orgString);
                }
              else if (orgString[0] != '#' &&
                       // Images and CSS links could be skipped
                       (!savedImages ||
                        (((attrType.AttrTypeNum != HTML_ATTR_SRC &&
                           attrType.AttrTypeNum != HTML_ATTR_data) ||
                           attrType.AttrSSchema != XHTMLSSchema) &&
                          ((elType.ElTypeNum != SVG_EL_image &&
                            elType.ElTypeNum != SVG_EL_PICTURE_UNIT)  ||
                           elType.ElSSchema != SVGSSchema))))
                {
                  newString = UpdateDocResource (doc, oldpath, newpath, resbase,
                                                 orgString, el, savedResources,
                                                 TRUE, fullCopy);
                  if (newString)
                    {
                      
                      /* register the modification to be able to undo it */
                      TtaRegisterAttributeReplace (attr, el, doc);
                      /* save the new attribute value */
                      TtaSetAttributeText (attr, newString, el, doc);
                      TtaFreeMemory (newString);
                    }
                  else
                    // no change
                    TtaFreeMemory (orgString);
                }
            }
          TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
        }
    }
}

/*----------------------------------------------------------------------
  InitSaveForm
  Build and display the Save As dialog box and prepare for input.
  ----------------------------------------------------------------------*/
static void InitSaveForm (Document document, View view, char *pathname)
{
  ThotBool         created, saveImgs, saveRes;

  if (TextFormat)
    {
      SaveAsHTML = FALSE;
      SaveAsXML = FALSE;
      SaveAsText = FALSE;
    }
  else if (IsXMLName (pathname) || DocumentMeta[document]->xmlformat)
    {
      SaveAsHTML = FALSE;
      SaveAsXML = TRUE;
      SaveAsText = FALSE;
    }
  else
    {
      SaveAsHTML = TRUE;
      SaveAsXML = FALSE;
      SaveAsText = FALSE;
    }
  TtaGetEnvBoolean ("COPY_IMAGES", &saveImgs);
  TtaGetEnvBoolean ("COPY_CSS", &saveRes);
  created = CreateSaveAsDlgWX (BaseDialog + SaveForm,
                               TtaGetViewFrame (document, view), pathname,
                               document, saveImgs, saveRes,
                               IsTemplateInstanceDocument(document));
  if (created)
    TtaShowDialogue (BaseDialog + SaveForm, FALSE, TRUE);
}


/*----------------------------------------------------------------------
  InitSaveObjectForm
  ----------------------------------------------------------------------*/
void InitSaveObjectForm (Document document, View view, char *object,
                         char *pathname)
{
  char           tempdir[MAX_LENGTH];
  ThotBool       created;

  if (Saving_lock)
    // there is a current saving operation
    return;

  SavingObject = document;
  strncpy (tempSavedObject, object, sizeof (tempSavedObject));

  TtaExtractName (pathname, tempdir, ObjectName);
  created = CreateSaveObject (BaseDialog + SaveForm,
                              TtaGetViewFrame (document, view), ObjectName);
}

/*----------------------------------------------------------------------
  DeleteTempObjectFile
  ----------------------------------------------------------------------*/
void DeleteTempObjectFile (void)
{
  TtaFileUnlink (tempSavedObject);
}


/*----------------------------------------------------------------------
  DoSaveObjectAs
  ----------------------------------------------------------------------*/
void DoSaveObjectAs (void)
{
  char           tempfile[MAX_LENGTH];
  char           pathname[MAX_LENGTH];
  char           docname[MAX_LENGTH];
  char           msg[MAX_LENGTH];
  int            res;
  ThotBool       dst_is_local;
   
  if (Saving_lock)
    // there is a current saving operation
    return;
  // start the saving operation
  Saving_lock = TRUE;

  /* @@ JK Testing to see if this part of the function is used elsewhere */
  /*
    dst_is_local = !IsW3Path (SavePath);
  */
  dst_is_local = TRUE;
  strcpy (tempfile, SavePath);
#ifndef _WX
  /* WX returns the whole path in SavePath */
  strcat (tempfile, DIR_STR);
  strcat (tempfile, ObjectName);
#endif /* _WX */
  if (!dst_is_local)
    {
      /* @@ We need to check the use of AMAYA_PREWRITE_VERIFY in this function*/
      /* @@ JK: add mime type  */
      res = PutObjectWWW (SavingObject, tempSavedObject, tempfile, NULL, NULL,
                          AMAYA_SYNC | AMAYA_NOCACHE |  AMAYA_FLUSH_REQUEST 
                          | AMAYA_USE_PRECONDITIONS, NULL, NULL);
      if (res)
        {
          Saving_lock = FALSE;
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + SaveForm, FALSE, TRUE);
          return;
        }
      SavingObject = 0;
      SavingDocument = 0;
      Saving_lock = FALSE;
      return;
    }

  Saving_lock = FALSE;
  if (TtaFileExist (tempfile))
    {
      /* ask confirmation */
      sprintf (msg, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
      InitConfirm (SavingObject, 1, msg);
      if (!UserAnswer)
        {
          /* the user has to change the name of the saving file */
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + SaveForm, FALSE, TRUE);
          // redisplay Save form
          res = SavingObject;
          SavingObject = 0;
          InitSaveObjectForm (res, 1, SavePath, ObjectName);
          return;
        }
    }
  else
    {
      TtaExtractName (tempfile, pathname, docname);
      if (!TtaCheckMakeDirectory (pathname, TRUE))
        {
          // cannot create the current path
          /* cannot save */
          sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempfile);
          InitInfo (NULL, msg);
          // redisplay Save form
          res = SavingObject;
          SavingObject = 0;
          InitSaveObjectForm (res, 1, SavePath, ObjectName);
          return;
        }
    }

  if (!TtaFileCopy (tempSavedObject, tempfile))
    {
      /* cannot save */
      sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempfile);
      InitInfo (NULL, msg);
      // redisplay Save form
      res = SavingObject;
      SavingObject = 0;
      InitSaveObjectForm (res, 1, SavePath, ObjectName);
      return;
    }
  /* delete the temporary file */
  DeleteTempObjectFile ();
  SavingObject = 0;
  SavingDocument = 0;
}

/*----------------------------------------------------------------------
  SaveDocumentAs                                              
  Entry point called when the user selects the SaveAs function
  ----------------------------------------------------------------------*/
void SaveDocumentAs (Document doc, View view)
{
  char                tempname[MAX_LENGTH];
  int                 i;
  char               *defaultName;

  if (DocumentURLs[doc] == 0)
    return;
  /* Protection against multiple invocations of this function */
  if (Saving_lock)
    {
      sprintf (tempname, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
      InitInfo (NULL, tempname);
      // there is a current saving operation
      return;
    }

  if (SavingDocument || SavingObject)
    // close the previous saving dialog
    CallbackDialogue (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);

  TextFormat = (DocumentTypes[doc] == docText ||
                DocumentTypes[doc] == docCSS ||
                DocumentTypes[doc] == docSource);
  
  /*
  ** initialize the user MIME type and charset global variables 
  */
  /* if there's no MIME type for this document, infer one */
  if (DocumentMeta[doc] && DocumentMeta[doc]->content_type)
    strcpy (UserMimeType, DocumentMeta[doc]->content_type);
  else if (DocumentTypes[doc] == docImage)
    strcpy (UserMimeType, DocImageMimeType (doc));
  else
    {
      if (DocumentTypes[doc] == docHTML)
        {
          if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat &&
              AM_UseXHTMLMimeType () )
            strcpy (UserMimeType, AM_XHTML_MIME_TYPE);
          else
            strcpy (UserMimeType, "text/html");
        }
      else if (DocumentTypes[doc] == docText)
        strcpy (UserMimeType, "text/plain");
      else if (DocumentTypes[doc] == docSVG)
        strcpy (UserMimeType, AM_SVG_MIME_TYPE);
      else if (DocumentTypes[doc] == docMath)
        strcpy (UserMimeType, AM_MATHML_MIME_TYPE);
      else if (DocumentTypes[doc] == docXml || DocumentTypes[doc] == docTemplate)
        strcpy (UserMimeType, AM_GENERIC_XML_MIME_TYPE);
      else if (DocumentTypes[doc] == docCSS)
        strcpy (UserMimeType, "text/css");
      else
        UserMimeType[0] = EOS;
    }

  /* charset */
  if (DocumentMeta[doc] && DocumentMeta[doc]->charset)
    strcpy (UserCharset, DocumentMeta[doc]->charset);
  else if (DocumentTypes[doc] == docCSS || DocumentTypes[doc] == docText)
    strcpy (UserCharset, "iso-8859-1");
  else
    UserCharset[0] = EOS;

  /* memorize the current document */
  if (SavingDocument == 0)
    {
      SavingDocument = doc;
      strcpy (tempname, DocumentURLs[doc]);
      /* suppress compress suffixes from tempname */
      i = strlen (tempname) - 1;
      if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
        {
          tempname[i-2] = EOS;
          TtaFreeMemory (DocumentURLs[doc]);
          DocumentURLs[doc] = TtaStrdup (tempname);
        }
      else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
        {
          tempname[i-1] = EOS;
          TtaFreeMemory (DocumentURLs[doc]);
          DocumentURLs[doc] = TtaStrdup (tempname);
        }

      /* if it is a Web document use the current SavePath */
      if (IsW3Path (tempname))
        {
          TtaExtractName (tempname, SavePath, SaveName);
          if (DocumentMeta[doc] && DocumentMeta[doc]->content_location)
            {
              /* use content-location instead of the loading name */
              strcpy (SaveName, DocumentMeta[doc]->content_location);
              strcpy (tempname, SavePath);
              strcat (tempname, URL_STR);
              strcat (tempname, SaveName);
            }
          else if (SaveName[0] == EOS)
            {
              defaultName = TtaGetEnvString ("DEFAULTNAME");
              if (defaultName == NULL || *defaultName == EOS)
                strcpy (SaveName, StdDefaultName);
              else
                strcpy (SaveName, defaultName);
              strcat (tempname, SaveName);
            }
	  
          /* add the suffix .html for HTML documents */
          if (!TextFormat &&
              DocumentTypes[SavingDocument] != docMath &&
              DocumentTypes[SavingDocument] != docSVG &&
              DocumentTypes[SavingDocument] != docImage &&
              !IsHTMLName (SaveName) && !IsXMLName (SaveName))
            {
              strcat (SaveName, ".html");
              strcpy (tempname, SavePath);
              strcat (tempname, URL_STR);
              strcat (tempname, SaveName);
            }
        }
      else
        TtaExtractName (tempname, SavePath, SaveName);
      TtaSetDialoguePosition ();
    }
  else
    {
      strcpy (tempname, SavePath);
      strcat (tempname, DIR_STR);
      strcat (tempname, SaveName);
    }

  /* display the dialog box */
  RemoveTemplate = FALSE; // don't remove template elements
  InitSaveForm (doc, 1, tempname);
}
   

/*----------------------------------------------------------------------
  UpdateDocumentCharset creates or updates the charset of the document.
  Return the allocated string of the charset.
  ----------------------------------------------------------------------*/
char *UpdateDocumentCharset (Document doc)
{
  Element		docEl;
  ElementType		elType;
  Attribute		charsetAttr;
  AttributeType	attrType;
  CHARSET              charset;
  const char          *ptr;
#define MAX_CHARSET_LEN 50
  char                *charsetname;
  ThotBool             oldStructureChecking;

  /* Create or update the document charset */
  charsetname = (char *)TtaGetMemory (MAX_CHARSET_LEN);
  charsetname[0] = EOS;
  charset = TtaGetDocumentCharset (doc);
  attrType.AttrTypeNum = 0;
  if (charset != UNDEFINED_CHARSET ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docSVG)
    {
      if (charset == UNDEFINED_CHARSET)
        strcat (charsetname, "utf-8");
      else
        {
          ptr = TtaGetCharsetName (charset);
          if (ptr)
            strcat (charsetname, ptr);
        }
      /* set the Charset attribute of the root element*/
      if (DocumentTypes[doc] == docHTML)
        attrType.AttrTypeNum = HTML_ATTR_Charset;
      else if (DocumentTypes[doc] == docMath)
        attrType.AttrTypeNum = MathML_ATTR_Charset;
      else if (DocumentTypes[doc] == docSVG)
        attrType.AttrTypeNum = SVG_ATTR_Charset;
#ifdef ANNOTATIONS
      else if (DocumentTypes[doc] == docAnnot)
        attrType.AttrTypeNum = Annot_ATTR_Charset;
#endif /* ANNOTATIONS */
      docEl = TtaGetMainRoot (doc);
      elType = TtaGetElementType (docEl);
      attrType.AttrSSchema = elType.ElSSchema;
      charsetAttr = TtaGetAttribute (docEl, attrType); 
       
      if (charsetAttr)
        /* Modify the charset attribute */
        TtaSetAttributeText (charsetAttr, charsetname, docEl, doc);	
      else
        {
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          charsetAttr = TtaNewAttribute (attrType);
          TtaAttachAttribute (docEl, charsetAttr, doc);
          TtaSetAttributeText (charsetAttr, charsetname, docEl, doc);	
          TtaSetStructureChecking (oldStructureChecking, doc);
        }
    }
  else if (charset == UNDEFINED_CHARSET)
     strcat (charsetname, TtaGetEnvString ("DOCUMENT_CHARSET"));
  return charsetname;
}

/*----------------------------------------------------------------------
  SetNamespacesAndDTD
  Whatever the document type, set the content of the Charset attribute
  (on the root element) according to the encoding used in the document.
  For (X)HTML documents, set the content of the Namespaces attribute
  (on the root element) according to the SSchemas used in the document;
  create a META element to specify Content-Type and Charset.
  removeTemplate says if the template namespace must be removed or not.
  ----------------------------------------------------------------------*/
void SetNamespacesAndDTD (Document doc, ThotBool removeTemplate)
{
  Element       root, el, head, meta, docEl, doctype, elFound, text;
  Element       next, elDecl;
  ElementType   elType;
  Attribute     attr;
  AttributeType	attrType, attrType1, attrType2;
  SSchema       nature;
  Language      lang;
  char         *ptr, *s;
  char         *charsetname = NULL;
  char          buffer[300];
  char         *attrText;
  int           length, profile, extraProfile, pi_type;
  ThotBool      useMathML, useSVG, useHTML, useXML, mathPI, useXLink;
  ThotBool      xmlDecl, xhtml_mimetype, insertMeta;

  insertMeta = FALSE;
  useMathML = FALSE;
  useHTML = FALSE;
  useSVG = FALSE;
  useXML = FALSE;
  useXLink = FALSE;
  nature = NULL;
  doctype = NULL; /* no DOCTYPE */
  elDecl = NULL;
  if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docSource)
    return;
#ifdef BOOKMARKS
  else if (DocumentTypes[doc] == docBookmark)
    return;
#endif /* BOOKMARKS */
#ifdef ANNOTATIONS
  else if (DocumentTypes[doc] == docAnnot)
    /* in an annotation, the body of the annotation corresponds to the
       root element we would normally get */
    root = ANNOT_GetHTMLRoot (doc, TRUE);
#endif /* ANNOTATIONS */
  else if (DocumentTypes[doc] == docXml)
    root = TtaGetMainRoot (doc);
  else
    root = TtaGetRootElement (doc);

  if (removeTemplate)
    // remove the XTiger namespace
    TtaRemoveANamespaceDeclaration (doc, root, NULL, Template_URI);

  /* Look for all natures used in the document */
  if (DocumentMeta[doc] && DocumentMeta[doc]->compound)
    do
      {
        TtaNextNature (doc, &nature);
        if (nature)
          {
            /* A nature is found, is it used ? */
            elFound = TtaSearchElementBySchema (nature, root);
            if (elFound != NULL)
              {
                ptr = TtaGetSSchemaName (nature);
                if (!strcmp (ptr, "MathML"))
                  useMathML = TRUE;
                else if (!strcmp (ptr, "SVG"))
                  useSVG = TRUE;
                else if (!strcmp (ptr, "XML"))
                  useXML = TRUE;
                else if (!strcmp (ptr, "HTML"))
                  useHTML = TRUE;
                else if (!strcmp (ptr, "XLink"))
                  useXLink = TRUE;
              }
          }
      }
    while (nature);
   
  docEl = TtaGetMainRoot (doc);
  /* a PI is generated when the XHTML document includes math elements and
     doesn't include a DOCTYPE */
  mathPI = useMathML && DocumentMeta[doc]->xmlformat;
  if (mathPI)
    // check if the user wants to generate this mathPI
    TtaGetEnvBoolean ("GENERATE_MATHPI", &mathPI);

  /* check if the document has a DOCTYPE declaration */
#ifdef ANNOTATIONS
  if (DocumentTypes[doc]  == docAnnot)
    {
      elType.ElSSchema = TtaGetSSchema ("HTML", doc);
      pi_type = HTML_EL_XMLPI;
    }
  else
#endif /* ANNOTATIONS */
    elType = TtaGetElementType (docEl);
  s = TtaGetSSchemaName (elType.ElSSchema);
  attrType.AttrSSchema = elType.ElSSchema;
  if (strcmp (s, "HTML") == 0)
    {
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      pi_type = HTML_EL_XMLPI;
      attrType.AttrTypeNum = HTML_ATTR_PI;
    }
#ifdef _SVG
  else if (strcmp (s, "SVG") == 0)
    {
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      pi_type = SVG_EL_XMLPI;
      attrType.AttrTypeNum = 0;
    }
#endif /* _SVG */
  else if (strcmp (s, "MathML") == 0)
    {
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      pi_type = MathML_EL_XMLPI;
      attrType.AttrTypeNum = 0;
    }
  else if (strcmp (s, "Template") == 0)
    {
      elType.ElTypeNum = 0;
      pi_type = Template_EL_XMLPI;
      attrType.AttrTypeNum = 0;
    }
  else
    {
      elType.ElTypeNum = XML_EL_doctype;
      pi_type = XML_EL_xmlpi;
      attrType.AttrTypeNum = 0;
    }
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);

  /* check if the compound document requests a DOCTYPE declaration */
  if (DocumentMeta[doc]->xmlformat && DocumentTypes[doc]  != docAnnot)
    {
      profile = TtaGetDocumentProfile(doc);
      extraProfile = TtaGetDocumentExtraProfile(doc);
      if (DocumentTypes[doc] == docHTML && doctype)
        {
          /* Create a XHTML + MathML + SVG doctype */
          if ((useMathML || useSVG) && !useXML && !useXLink && profile == L_Xhtml11)
            {
              CreateDoctype (doc, doctype, L_Xhtml11, extraProfile, useMathML, useSVG, FALSE);
              /* it's not necessary to generate the math PI */
              mathPI = FALSE;
            }
          else if (useMathML || useSVG || useXML)
            /* remove the current doctype */
            TtaDeleteTree (doctype, doc);
          else
            // regenerate the doctype with the right profile
            CreateDoctype (doc, doctype, profile, extraProfile, useMathML, useSVG, FALSE);
        }
      else if (doctype &&
               ((useSVG && (useMathML || useHTML || useXML || useXLink)) ||
                (useXML && (useMathML || useHTML || useSVG || useXLink))))
        /* several namespaces: remove the current doctype */
        TtaDeleteTree (doctype, doc);
    }
   
  /* Create or update the document charset */
  charsetname = UpdateDocumentCharset (doc);
  if (attrType.AttrTypeNum != 0)
    {
      /* delete the previous PI attribute */
      attr = TtaGetAttribute (root, attrType);
      if (attr)
        TtaRemoveAttribute (root, attr, doc);
    }

  /* check if XML declaration or the math PI are already defined */
  xmlDecl = DocumentMeta[doc]->xmlformat;
  if (DocumentTypes[doc]  != docAnnot)
    {
      el = TtaGetFirstChild (docEl);
      while (el)
	{
	  elType = TtaGetElementType (el);
	  if (elType.ElTypeNum != pi_type)
	    TtaNextSibling (&el);
	  else
	    {
	      /* get PI lines */
	      next = el;
	      TtaNextSibling (&next);
	      elFound = TtaGetFirstChild (el);
	      while (elFound)
		{
		  /* get PI text */
		  text = TtaGetFirstChild (elFound);
		  if (text == NULL)
		    text = elFound;
		  length = 300;
		  TtaGiveTextContent (text, (unsigned char *)buffer, &length,
				      &lang);
		  if (strstr (buffer, "xml version="))
		    {
		      if (strstr (buffer, charsetname))
			{
			  /* not necessary to generate the XML declaration */
			  xmlDecl = FALSE;
			  elDecl = el;
			}
		      else
			{
			  // the charset changed -> regenerate the declaration
			  xmlDecl = TRUE;
			  elDecl = NULL;
			  TtaDeleteTree (el, doc);
			  el = next;
			}
		      elFound = NULL;
		    }
		  else if (strstr (buffer, "mathml.xsl"))
		    {
		      if (!mathPI)
			{
			  if (el)
			    // this PI must be removed
			    TtaDeleteTree (el, doc);
			}
		      /* it's not necessary to generate the math PI */
		      mathPI = FALSE;
		      elFound = NULL;
		    }
		  else if (RemoveTemplate && strstr (buffer, "xtiger template"))
		    TtaDeleteTree (el, doc);
		  if (elFound)
		    TtaNextSibling (&elFound);
		}
	      el = next;
	    }
	}

      if (xmlDecl)
        {
#ifdef IV
          if (DocumentTypes[doc]  == docHTML)
            // according to new specs don't generate it
            xmlDecl = FALSE;
          else
#endif
            // check if the user wants to generate xml declaration
            TtaGetEnvBoolean ("USE_XML_DECLARATION", &xmlDecl);
        }
      if (xmlDecl && charsetname[0] != EOS)
	{
	  /* generate the XML declaration */
	  /* Check the Thot abstract tree against the structure schema. */
	  TtaSetStructureChecking (FALSE, doc);
	  elType.ElTypeNum = pi_type;
	  el = TtaNewTree (doc, elType, "");
	  elDecl = el;
	  TtaInsertFirstChild (&el, docEl, doc);
	  elFound = TtaGetFirstChild (el);
	  text = TtaGetFirstChild (elFound);
	  strcpy (buffer, "xml version=\"1.0\" encoding=\"");
	  strcat (buffer, charsetname);
	  strcat (buffer, "\"");
	  TtaSetTextContent (text, (unsigned char*)buffer,  Latin_Script, doc);
	  TtaSetStructureChecking (TRUE, doc);
	}
      if (mathPI)
	{
	  /* generate the David Carliste's xsl stylesheet for MathML */
	  /* Check the Thot abstract tree against the structure schema. */
	  TtaSetStructureChecking (FALSE, doc);
	  elType.ElTypeNum = pi_type;
	  el = TtaNewTree (doc, elType, "");
	  if (elDecl)
	    TtaInsertSibling(el,elDecl, FALSE, doc );
	  else
	    TtaInsertFirstChild (&el, docEl, doc);
	  elFound = TtaGetFirstChild (el);
	  text = TtaGetFirstChild (elFound);
	  strcpy (buffer, MATHML_XSLT_URI);
	  strcat (buffer, MATHML_XSLT_NAME);
	  strcat (buffer, "\"");
	  TtaSetTextContent (text, (unsigned char*)buffer,  Latin_Script, doc);
	  TtaSetStructureChecking (TRUE, doc);
	}
    }

  /* Create or update a META element to specify Content-type and Charset */
  if (
#ifdef ANNOTATIONS
      (DocumentTypes[doc] == docAnnot && ANNOT_bodyType (doc) == docHTML) ||
#endif /* ANNOTATIONS */
      DocumentTypes[doc] == docHTML)
    {
      attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
      if (attrType.AttrSSchema)
        {
          el = TtaGetFirstChild (root);
          head = NULL;
          while (el && !head)
            {
              elType = TtaGetElementType (el);
              if (elType.ElSSchema == attrType.AttrSSchema &&
                  elType.ElTypeNum == HTML_EL_HEAD)
                head = el;
              else
                TtaNextSibling (&el);
            }

          if (head)
            {
              /* indicate the MIME type and the charset in a meta element with
                 an http-equiv attr as requested in the XHTML specification */
              /* look for a meta/http-equiv element */
              el = TtaGetFirstChild (head);
              meta = NULL;
              attrType.AttrTypeNum = HTML_ATTR_http_equiv;
              attr = NULL;
              attrType1.AttrSSchema = attrType.AttrSSchema;
              attrType1.AttrTypeNum = HTML_ATTR_property;
              attrType2.AttrSSchema = attrType.AttrSSchema;
              attrType2.AttrTypeNum = HTML_ATTR_meta_name;
              while (el)
                {
                  elType = TtaGetElementType (el);
                  next = el;
                  TtaNextSibling (&next);
                  if (elType.ElSSchema == attrType.AttrSSchema &&
                      elType.ElTypeNum == HTML_EL_META)
                    {
                      attr = TtaGetAttribute (el, attrType);
                      if (attr)
                        {
                          length = TtaGetTextAttributeLength (attr);
                          attrText = (char *)TtaGetMemory (length + 1);
                          TtaGiveTextAttributeValue (attr, attrText, &length);
                          if (!strcasecmp (attrText, "content-type"))
                            {
                              if (meta)
                                // there is a previous http-equiv meta
                                TtaDeleteTree (el, doc);
                              else
                                meta = el;
                            }
                          TtaFreeMemory (attrText);
                        }
                      else
                        {
                          attr = TtaGetAttribute (el, attrType1);
                          if (attr == NULL)
                            attr = TtaGetAttribute (el, attrType2);
                          if (attr == NULL)
                            // a meta with only a content
                            TtaDeleteTree (el, doc);                          
                        }
                    }
                    el = next;
                }
              if (charsetname[0] != EOS )
                {
                  if (!meta)
                    {
                      /* there is no meta element with a http-equiv attribute */
                      /* create one at the begginning of the head */
                      elType.ElSSchema = attrType.AttrSSchema;
                      elType.ElTypeNum = HTML_EL_META;
                      meta = TtaNewElement (doc, elType);
                      /* do not insert the meta element yet. Wait for its
                         attribute to be created, otherwise mandatory attributes
                         will prompt the user with no reason */
                      insertMeta = TRUE;
                      attr = NULL;
                    }
                  if (!attr)
                    {
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (meta, attr, doc);
                    }
                  TtaSetAttributeText (attr, "content-type", meta, doc);

                  attrType.AttrTypeNum = HTML_ATTR_meta_content;
                  attr = TtaGetAttribute (meta, attrType);
                  if (!DocumentMeta[doc] || !DocumentMeta[doc]->xmlformat)
                    /* must be text/html */
                    xhtml_mimetype = FALSE; 
                  else if (DocumentMeta[doc]->content_type == NULL)
                    {
                      if (attr)
                        {
                          length = TtaGetTextAttributeLength (attr);
                          attrText = (char *)TtaGetMemory (length + 1);
                          TtaGiveTextAttributeValue (attr, attrText, &length);
                          if (!strncmp (attrText, "text/html", 9))
                            xhtml_mimetype = FALSE;
                          else
                            xhtml_mimetype = TRUE;
                          TtaFreeMemory (attrText);
                        }
                      else
                        /* what default MIME type for the html document */
                        TtaGetEnvBoolean ("ENABLE_XHTML_MIMETYPE", &xhtml_mimetype);
                    }
                  if (!attr)
                    {
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (meta, attr, doc);
                    }
                  /* all attributes have been attached to the element.
                     We can insert it in the tree now */
                  if (insertMeta)
                    TtaInsertFirstChild (&meta, head, doc);
                  if (DocumentMeta[doc] && DocumentMeta[doc]->content_type)
                    strcpy (buffer, DocumentMeta[doc]->content_type);
                  else if (xhtml_mimetype)
                    strcpy (buffer, AM_XHTML_MIME_TYPE);
                  else
                    strcpy (buffer, "text/html");
                  if (charsetname[0] != EOS)
                    {
                      strcat (buffer, "; charset=");
                      strcat (buffer, charsetname);
                      TtaSetAttributeText (attr, buffer, meta, doc);
                    }
                }
            }
        } 
    }
  TtaFreeMemory (charsetname);
}

/*----------------------------------------------------------------------
  ParseWithNewDoctype
  Parse a temporary saved version of the document to detect
  the parsing errors due to the new doctype
  ----------------------------------------------------------------------*/
ThotBool ParseWithNewDoctype (Document doc, char *localFile, char *tempdir,
                              char *documentname, int new_doctype, int new_extraProfile,
                              ThotBool *error, ThotBool xml_doctype,
                              ThotBool useMathML, ThotBool useSVG)
{
  SSchema       schema;
  CHARSET       charset;
  DocumentType  thotType;
  Document      ext_doc = 0;
  ElementType   elType;
  Element       docEl, eltype;
  char          charsetname[MAX_LENGTH];
  int           parsingLevel, extraProfile;
  char         *s;
  char          type[NAME_LENGTH];
  char          tempdoc2 [100];
  char          err_doc [100];
  char          err_extdoc [100];
  ThotBool      xmlDec, withDoctype, isXML, useMath, isKnown;
  ThotBool      ok = FALSE;

  /* Clean up previous Parsing errors file */
  CleanUpParsingErrors ();

  /* Remove the Parsing errors file */
  RemoveParsingErrors (doc);

  schema = TtaGetDocumentSSchema (doc);
  s = TtaGetSSchemaName (schema);
  strcpy ((char *)type, s);
  ext_doc = TtaNewDocument (type, "tmp");
  if (ext_doc == 0)
    return (ok);
  else
    {
      DocumentMeta[ext_doc] = DocumentMetaDataAlloc ();
      strcat (type, "P");
      TtaSetPSchema (ext_doc, type);
      if (DocumentURLs[ext_doc])
        {
          TtaFreeMemory (DocumentURLs[ext_doc]);
          DocumentURLs[ext_doc] = NULL;
        }
      DocumentTypes[ext_doc] = DocumentTypes[doc];
      DocumentURLs[ext_doc] = TtaStrdup (DocumentURLs[doc]);
      DocumentMeta[ext_doc]->form_data = TtaStrdup (DocumentMeta[doc]->form_data);
      DocumentMeta[ext_doc]->initial_url = TtaStrdup (DocumentMeta[doc]->initial_url);
      DocumentMeta[ext_doc]->method = DocumentMeta[doc]->method;
      DocumentSource[ext_doc] = 0;
      DocumentMeta[ext_doc]->charset = TtaStrdup (DocumentMeta[doc]->charset);
      DocumentMeta[ext_doc]->xmlformat = xml_doctype;
      charset = TtaGetDocumentCharset (doc);
      TtaSetDocumentCharset (ext_doc, charset, FALSE);
      TtaSetDocumentProfile (ext_doc, new_doctype, new_extraProfile);

      /* Copy the current document into a second temporary file */
      sprintf (tempdoc2, "%s%c%d%c%s",
               TempFileDirectory, DIR_SEP, ext_doc, DIR_SEP, documentname);
      if (!DocumentMeta[doc]->xmlformat && xml_doctype)
        //convert HTML into XHTML
        TtaExportDocumentWithNewLineNumbers (doc, tempdoc2, "HTMLTX", FALSE);
      else
        TtaFileCopy (localFile, tempdoc2);
    }
  
  /* Check if there is a doctype declaration */
  charsetname[0] = EOS;
  CheckDocHeader (localFile, &xmlDec, &withDoctype, &isXML, &useMath, &isKnown,
                  &parsingLevel, &charset, charsetname, &thotType, &extraProfile);
  /* Store the new document type */
  TtaSetDocumentProfile (ext_doc, new_doctype, new_extraProfile);

  // Get user information about read IDs
  Check_read_ids = TRUE;
  /* Calls the right parser */
  if (DocumentMeta[ext_doc]->xmlformat)       
    StartXmlParser (ext_doc, localFile, documentname, tempdir,
                    localFile, xmlDec, withDoctype, useMath, TRUE);
  else
    StartParser (ext_doc, localFile, documentname, tempdir, localFile, FALSE, TRUE);
  
  /* Check parsing errors */
  if (ErrFile &&
      (XMLCharacterNotSupported || XMLInvalidToken || XMLNotWellFormed ||
       XMLErrorsFoundInProfile || XMLErrorsFound))
    {
      CleanUpParsingErrors ();
      /* Show the parsing errors */
      sprintf (err_doc, "%s%c%d%cPARSING.ERR",
               TempFileDirectory, DIR_SEP, doc, DIR_SEP);
      sprintf (err_extdoc, "%s%c%d%cPARSING.ERR",
               TempFileDirectory, DIR_SEP, ext_doc, DIR_SEP);
      TtaFileCopy (err_extdoc, err_doc);
      ShowLogFile (doc, 1);
      ShowSource (doc, 1);
      /* Ask for confirmation */
      InitConfirm3L (doc, 1,
                     TtaGetMessage (AMAYA, AM_CHANGE_DOCTYPE1),
                     TtaGetMessage (AMAYA, AM_CHANGE_DOCTYPE2),
                     NULL,
                     TRUE);
      ok =  UserAnswer;
      *error = TRUE;
      TtaFileUnlink (err_doc);
    }
  else
    {
      if (ErrFile)
        CleanUpParsingErrors ();
      ok = TRUE;
    }
  /* Update the original document */
  if (ok)
    {
      /* Remove the previous doctype if it exists */
      docEl = TtaGetMainRoot (ext_doc);
      elType = TtaGetElementType (docEl);
      if (xmlDec && !xml_doctype && DocumentTypes[doc] == docHTML)
        {
          // remove the xml declaration
          elType.ElTypeNum = HTML_EL_XMLPI;
          eltype = TtaSearchTypedElement (elType, SearchInTree, docEl);
          TtaDeleteTree (eltype, ext_doc);
        }
      /* Search the doctype declaration according to the main schema */
      if (new_doctype == L_Basic || new_doctype == L_Strict ||
          new_doctype == L_Xhtml11 || new_doctype == L_Transitional)
        elType.ElTypeNum = HTML_EL_DOCTYPE;
      else if (new_doctype == L_MathML) 
        elType.ElTypeNum = MathML_EL_DOCTYPE;
      else if (new_doctype == L_SVG) 
        elType.ElTypeNum = SVG_EL_DOCTYPE;
      eltype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      /* Add the new doctype */
      CreateDoctype (ext_doc, eltype, new_doctype, new_extraProfile, useMathML, useSVG, TRUE);

      /* Save this new document state */
      if (DocumentTypes[doc] == docSVG)
        TtaExportDocumentWithNewLineNumbers (ext_doc, localFile, "SVGT", FALSE);
      else if (DocumentTypes[doc] == docMath)
        TtaExportDocumentWithNewLineNumbers (ext_doc, localFile, "MathMLT", FALSE);
      else if (new_doctype == L_Xhtml11 || new_doctype == L_Basic)
        TtaExportDocumentWithNewLineNumbers (ext_doc, localFile, "HTMLT11", FALSE);
      else if (xml_doctype)
        TtaExportDocumentWithNewLineNumbers (ext_doc, localFile, "HTMLTX", FALSE);
      else
        TtaExportDocumentWithNewLineNumbers (ext_doc, localFile, "HTMLT", FALSE);

      /* reparse the document */
      DocumentMeta[doc]->xmlformat = xml_doctype;
      RestartParser (doc, localFile, tempdir, documentname, FALSE, TRUE);
      /* Notify the document as modified */
      TtaSetDocumentModified (doc);
      /* Synchronize the document */
      Synchronize (doc, 1);
    }

  /* Delete the external document */
  if (ext_doc != 0)
    {
      FreeDocumentResource (ext_doc);
      TtaCloseDocument (ext_doc);
      TtaFreeMemory (DocumentURLs[ext_doc]);
      DocumentURLs[ext_doc] = NULL;
      DocumentTypes[ext_doc] = docFree;
    }

  return (ok);
}

/*----------------------------------------------------------------------
  RestartParser
  check_ids is TRUE when unique ids must be checked
  ----------------------------------------------------------------------*/
void RestartParser (Document doc, char *localFile,
                    char *tempdir, char *documentname, ThotBool show_errors,
                    ThotBool check_ids)
{
  CHARSET       charset, doc_charset;
  DocumentType  thotType;
  char          charsetname[MAX_LENGTH];
  int           profile, parsingLevel, extraProfile, old_extraProfile;
  ThotBool      xmlDec, withDoctype, isXML, useMath, isKnown;

  if (doc == 0)
    return;
  /* Clean up previous Parsing errors file */
  CleanUpParsingErrors ();
  /* Remove the Parsing errors file */
  RemoveParsingErrors (doc);
  /* Remove the previous namespaces declaration */
  TtaFreeNamespaceDeclarations (doc);
  TtaSetDisplayMode (doc, NoComputedDisplay);

  /* check if there is an XML declaration with a charset declaration */
  charsetname[0] = EOS;
  CheckDocHeader (localFile, &xmlDec, &withDoctype, &isXML, &useMath, &isKnown,
                  &parsingLevel, &charset, charsetname, &thotType, &extraProfile);
  /* Check charset information in a meta */
  if (charset == UNDEFINED_CHARSET)
    CheckCharsetInMeta (localFile, &charset, charsetname);
  doc_charset = TtaGetDocumentCharset (doc);
  if (charset != doc_charset)
    {
      /* Update the charset info */
      TtaSetDocumentCharset (doc, charset, FALSE);
      if (DocumentMeta[doc]->charset)
        {
          TtaFreeMemory (DocumentMeta[doc]->charset);
          DocumentMeta[doc]->charset = NULL;
        }
      if (charsetname[0] != EOS)
        DocumentMeta[doc]->charset = TtaStrdup (charsetname);
    }
  
  DocumentMeta[doc]->compound = FALSE;
  if (isXML || IsMathMLName (localFile) ||
      IsSVGName (localFile) || IsXMLName (localFile) ||
      DocumentTypes[doc] == docXml || DocumentTypes[doc] == docTemplate)
    DocumentMeta[doc]->xmlformat = TRUE;
  else
    DocumentMeta[doc]->xmlformat = FALSE;

  /* Removes all CSS informations linked with the document */
  RemoveDocCSSs (doc, TRUE);  
  /* Clear all editing operations registered in the editing history of the
     document */
  TtaClearUndoHistory (doc);
  TtaCancelSelection (doc);
  /* Free access keys table */
  TtaRemoveDocAccessKeys (doc);
  TtaClearUndoHistory (doc);
  /* Store the document profile if it has been modified */
  profile = TtaGetDocumentProfile (doc);
  old_extraProfile = TtaGetDocumentExtraProfile (doc);
  if ((profile != parsingLevel) || (old_extraProfile != extraProfile))
    {
      TtaSetDocumentProfile (doc, parsingLevel, extraProfile);
      TtaUpdateMenus (doc, 1, FALSE);
    }

  // Force the check of read IDs
  if (check_ids)
    Check_read_ids = TRUE;
  /* Calls the corresponding parser */
  if (DocumentMeta[doc]->xmlformat)       
    StartXmlParser (doc, localFile, documentname, tempdir,
                    localFile, xmlDec, withDoctype, useMath, FALSE);
  else
    StartParser (doc, localFile, documentname, tempdir, localFile, FALSE, FALSE);

  /* Restore the Display Mode */
  TtaSetDisplayMode (doc, DisplayImmediately);
#ifdef _WX
  // set the default icon
  TtaSetPageIcon (doc, 1, NULL);
#endif /* _WX */

  /* fetch and display all images referred by the document */
  ActiveTransfer (doc);
  FetchAndDisplayImages (doc, AMAYA_LOAD_IMAGE, NULL);
  ResetStop (doc);
#ifdef _GL
  if (FilesLoading[doc] == 0 &&
      TtaGetViewFrame (doc, 1) != 0)
    /* all files included in this document have been loaded and the
       document is displayed. Animations can be played now */
    TtaPlay (doc, 1);
#endif /* _GL */

  /* check parsing errors */
  if (show_errors)
    CheckParsingErrors (doc);
  else
    CloseLogs (doc);
}

/*----------------------------------------------------------------------
  RedisplaySourceFile
  If doc is a structured document and the source view is open, 
  redisplay the source.
  ----------------------------------------------------------------------*/
void RedisplaySourceFile (Document doc)
{
  Element             el;
  char               *localFile;
  char	              documentname[MAX_LENGTH];
  char	              tempdir[MAX_LENGTH];
  int		              position, distance;
  NotifyElement       event;

  if (IsXMLDocType (doc))
    /* it's a structured document */
    if (DocumentSource[doc])
      /* The source code of this document is currently displayed */
      {
        /* get the current position in the document */
        position = RelativePosition (DocumentSource[doc], &distance);
        TtaClearUndoHistory (DocumentSource[doc]);
        /* if selection is in the source document, cancel it */
        TtaUnselect (DocumentSource[doc]);
        /* Get its local copy */
        localFile = GetLocalPath (doc, DocumentURLs[doc]);
        TtaExtractName (localFile, tempdir, documentname);
        /* parse and display the new version of the source code */
        StartParser (DocumentSource[doc], localFile, documentname, tempdir,
                     localFile, TRUE, FALSE);
        /* Clear the document history because the document is reparsed */
        TtaClearUndoHistory (DocumentSource[doc]);
        TtaSetDocumentUnmodified (DocumentSource[doc]);

        /* restore the current position in the document */
        el = ElementAtPosition (DocumentSource[doc], position);
        TtaShowElement (DocumentSource[doc], 1, el, distance);
        event.document = doc;
        event.element = NULL;
        SynchronizeSourceView (&event);
        TtaSetDocumentName (DocumentSource[doc], documentname);
        SetWindowTitle (doc, DocumentSource[doc], 0);
        TtaFreeMemory (localFile);
      }
}

/*----------------------------------------------------------------------
  SaveDocumentLocally save the document in a local file.
  Return TRUE if the document has been saved
  ----------------------------------------------------------------------*/
static ThotBool SaveDocumentLocally (Document doc, char *directoryName,
                                     char *documentName)
{
  LoadedImageDesc    *pImage;
  char               *ptr;
  char                tempname[MAX_LENGTH];
  char                docname[100];
  ThotBool            ok;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);
#endif

  strcpy (tempname, directoryName);
  strcat (tempname, DIR_STR);
  strcat (tempname, documentName);
  ok = FALSE;
  if (SaveAsText)
    {
      /* the document will be exported without line numbers */
      SetInternalLinks (doc);
      if (DocumentTypes[doc] == docHTML)
        ok = TtaExportDocument (doc, tempname, "HTMLTT");
      else if (DocumentTypes[doc] == docSVG)
        ok = TtaExportDocument (doc, tempname, "SVGT");
      if (DocumentTypes[doc] == docMath)
        ok = TtaExportDocument (doc, tempname, "MathMLT");
    }
  else
    {
      if (DocumentTypes[doc] == docHTML)
        DocumentMeta[doc]->xmlformat = SaveAsXML;
      SetNamespacesAndDTD (doc, RemoveTemplate);
      if (DocumentTypes[doc] == docHTML)
        {
          if (SaveAsXML)
            {
              if (TtaGetDocumentProfile(doc) == L_Xhtml11 || TtaGetDocumentProfile(doc) == L_Basic)
                ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT11", RemoveTemplate);
              else
                ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLTX", RemoveTemplate);
            }
          else
            ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT", RemoveTemplate);
        }
      else if (DocumentTypes[doc] == docSVG)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "SVGT", RemoveTemplate);
      else if (DocumentTypes[doc] == docMath)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "MathMLT", RemoveTemplate);
      else if (DocumentTypes[doc] == docTemplate)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "TemplateT", FALSE);
      else if (DocumentTypes[doc] == docXml)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, NULL, RemoveTemplate);
      else if (DocumentTypes[doc] == docImage)
        {
          /* copy the image file to the new destination */
          if (!IsW3Path (DocumentURLs[doc]))
            /* local to local */
            ok = TtaFileCopy (DocumentURLs[doc], tempname);
          else
            {
              /* remote to local */
              /* copy the copy from the cache to its new destination and
                 updated the pImage description (or maybe just erase it? */
              pImage = SearchLoadedImageByURL (doc, DocumentURLs[doc]);
              if (pImage)
                {
                  /* copy the file */
                  TtaFileCopy (pImage->tempfile, tempname);
                  /* remove the old file (note that it's the local name that
                     we have to free, because we're using an HTML container
                     to show the image */
                  TtaFileUnlink (pImage->localName);
                  TtaFreeMemory (pImage->localName);
                  TtaFreeMemory (pImage->originalName);
                  /* save the new location */
                  pImage->originalName = (char *)TtaGetMemory (sizeof ("internal:")
                                                               + strlen (tempname)
                                                               + 1);
                  sprintf (pImage->originalName, "internal:%s", tempname);
                  pImage->localName = GetLocalPath (doc, tempname);
                  ok = TRUE;
                }
            }
        }

      if (ok)
        {
          TtaSetDocumentDirectory (doc, directoryName);
          strcpy (docname, documentName);
          /* Change the document name in all views */
          TtaSetDocumentName (doc, docname);
          SetWindowTitle (doc, doc, 0);
          /* save a local copy of the current document */
          ptr = GetLocalPath (doc, tempname);
          if (DocumentTypes[doc] == docImage)
            /* export the new container (but to the temporary file name */
            ok = TtaExportDocumentWithNewLineNumbers (doc, ptr, "HTMLTX", FALSE);
          else
            ok = TtaFileCopy (tempname, ptr);
          TtaFreeMemory (ptr);
        }
    }
  return (ok);
}

/*----------------------------------------------------------------------
  HasSavingName
  This function is called whenether one tries to save a document
  without name (just the directory path e.g. http://www.w3.org/pub/WWW/ )
  It asks the user whether an extra name suffix should be added or
  abort.
  Return TRUE if there is a document name
  with_suffix is TRUE if the document name has a suffix
  ----------------------------------------------------------------------*/
ThotBool HasSavingName (Document doc, View view, char *url, ThotBool *with_suffix)
{
  char            msg[MAX_LENGTH], suffix[MAX_LENGTH];
  char            documentname[MAX_LENGTH];
  int             len;
  ThotBool        ok;
  char           *defaultName;

  len = strlen (url);
  TtaExtractName (url, msg, documentname);
  if (documentname[0] != EOS)
    {
      // check if there is a suffix
      TtaExtractSuffix (documentname, suffix);
      *with_suffix = (suffix[0] != EOS);
      return (TRUE);
    }
  else
    {
      /* the name is not correct for the put operation */
      strcpy (msg, TtaGetMessage(AMAYA, AM_NO_NAME));
      strcat (msg, url);
      if (IsW3Path (url))
        {
          if (url[len -1] != URL_SEP)
            strcat (msg, URL_STR);
        }
      else if (url[len -1] != DIR_SEP)
        strcat (msg, DIR_STR);
      /* get default name */
      ok = (DocumentMeta[doc] && DocumentMeta[doc]->content_location);
      if (ok)
        // don't pay attention to current suffix
        *with_suffix = TRUE;
      else
        {
          defaultName = TtaGetEnvString ("DEFAULTNAME");
          if (defaultName == NULL || *defaultName == EOS)
            defaultName = (char*)StdDefaultName;
          strcat (msg, defaultName);
          InitConfirm (doc, view, msg);
          
          if (UserAnswer != 0 && DocumentMeta[doc])
            {
              DocumentMeta[doc]->content_location = TtaStrdup (defaultName);
              ok = (DocumentMeta[doc]->content_location != NULL);
            }
          if (ok)
            {
              // check isf there is a suffix
              strcpy (documentname, DocumentMeta[doc]->content_location);
              TtaExtractSuffix (documentname, suffix);
              *with_suffix = (suffix[0] != EOS);
            }
          else
            *with_suffix = FALSE;
        }
      return ok;
    }
}

/*-----------------------------------------------------------------------
  SaveWikiFile callback
  -----------------------------------------------------------------------*/
void SaveWiki_callback (int doc, int status, char *urlName, char *outputfile,
                        char *proxyName, AHTHeaders *http_headers, void * context)
{
  WIKI_context       *ctx;

  ctx = (WIKI_context *) context;
  if (!ctx)
    return;
  TtaFreeMemory (ctx->localfile);
  TtaFreeMemory (ctx->output);
  TtaFreeMemory (ctx);
  if (status < 0)
    {
      DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
      ResetStop (doc);
      if (AmayaLastHTTPErrorMsg[0] == EOS)
        sprintf (AmayaLastHTTPErrorMsg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE),
                 DocumentURLs[doc]);
      InitInfo ("", AmayaLastHTTPErrorMsg);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  GiveProtertyValue looks for the value of the given porperty within
  the document.
  value is the allocated string that will receive the value and length
  the length of that string.
  Return:
  - value into the value paramerter
  - the length of the reurned value
  ----------------------------------------------------------------------*/
void GiveProtertyValue (Document doc, char *property, char *value, int *length)
{
  Element	            root, el;
  ElementType        elType;
  AttributeType       attrpType, attrcType;
  Attribute	          attrp, attrc;
  SearchDomain        scope = SearchInTree;
  char                buffer[MAX_LENGTH];
  int                 l;
  ThotBool            found = FALSE;

  root = TtaGetMainRoot (doc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_HEAD;
  root = TtaSearchTypedElement (elType, SearchInTree, root);
  if (root)
    {
      attrpType.AttrSSchema = elType.ElSSchema;
      attrcType.AttrSSchema = elType.ElSSchema;
      if (!attrpType.AttrSSchema)
        return;

      // look for the right property attribute
      attrpType.AttrTypeNum = HTML_ATTR_property;
      attrcType.AttrTypeNum = HTML_ATTR_meta_content;
      while (root && !found)
        {
          TtaSearchAttribute (attrpType, scope, root, &el, &attrp);
          if (attrp)
            {
              l = MAX_LENGTH - 1;
              TtaGiveTextAttributeValue (attrp, buffer, &l);
              found = (strcmp (buffer, property) == 0);
              if (found)
                {
                  // the property is found out
                  root = NULL;
                  attrc = TtaGetAttribute (el, attrcType);
                  if (attrc)
                    {
                      TtaGiveTextAttributeValue (attrc, value, length);
                      return;
                    }
                }
              else
                // it's not the right property
                root = el;
              scope = SearchForward;
            }
          else
            root = NULL;
        }
    }
  // default return
  value[0] = EOS;
  *length = 0;
}

/*----------------------------------------------------------------------
  SafeSaveFileThroughNet
  Send a file through the Network (using the PUT HTTP method) and double
  check for errors using a following GET.
  Return:
  0 if the file has been saved,
  1 if the save failed, 
  2 if nothing is done
  ----------------------------------------------------------------------*/
static int SaveWikiFile (Document doc, char *localfile,
                         char *remotefile, const char *content_type,
                         ThotBool use_preconditions)
{
#define PARAM_INCREMENT 100
  WIKI_context       *ctx;
  char               *server, *url, *params, *end, *ptr;
  char                prop[PARAM_INCREMENT], value[MAX_LENGTH];
  int                 len, l, res = 2;

  server = TtaGetEnvString ("WIKI_SERVER");
  len = strlen(server);
  if (len && !strncmp (remotefile, server, len))
    {
      /* Save */
      url = TtaGetEnvString ("WIKI_POST_URI");
      params = TtaGetEnvString ("WIKI_POST_PARAMS");
      if (url == NULL)
        // not enough resource
         return 1;

      FormLength = strlen (url) + PARAM_INCREMENT;
      FormBuf = (char *)(char *)TtaGetMemory (FormLength);
      FormBuf[0] = EOS;
      AddToBuffer (url);
      if (params)
        {
          if (params[0] != '?' && strstr (url, "?") == NULL)
            AddToBuffer ("?");
          // look for dynamic parameters
          while (params && *params != EOS)
            {
               ptr = strstr (params, "=");
              if (ptr)
                {
                  end = strstr (ptr, "&");
                  if (end == NULL)
                    end = &ptr[strlen (ptr)];
                }
              else
                end = NULL;
              if (end)
                {
                  // add a property and its value
                  len = (int)(ptr - params) + 1;
                  if (len >= MAX_LENGTH)
                    len = MAX_LENGTH - 1;
                  strncpy (prop, params, len);
                  prop[len] = EOS;
                  AddToBuffer (prop);

                  // look for the property value in the document
                  len = (int)(end - ptr) - 1;
                  if (len >= MAX_LENGTH)
                    len = MAX_LENGTH - 1;
                  strncpy (prop, &ptr[1], len);
                  prop[len] = EOS;
                  l = MAX_LENGTH - 1;
                  GiveProtertyValue (doc, prop, value, &l);
                  if (l == 0)
                    // use the default property value
                    AddToBuffer (prop);
                  else
                    AddToBuffer (value);

                    // move to the next parameter
                    params = end;
                }
              else
                // use the default property value
                AddToBuffer (params);
            }
        }

      /* launch the request */
      //ActiveTransfer (doc);
      /* create the context for the callback */
      ctx = (WIKI_context*)TtaGetMemory (sizeof (WIKI_context));
      //memset (ctx,  0, sizeof (WIKI_context));
      ctx->localfile = TtaStrdup (DocumentURLs[doc]);
      ctx->output = (char *)TtaGetMemory (MAX_LENGTH); // requested by GetObjectWWW
      ctx->output[0]  = EOS;
      res = GetObjectWWW (doc, doc, FormBuf, localfile,
                           ctx->output,
                          AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
                          NULL, NULL, 
                          (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))  SaveWiki_callback,
                          (void *) ctx, NO, "multipart/form-data");
      // The request is now done
      FormLength = 0;
      TtaFreeMemory (FormBuf);
      FormBuf = NULL;
    }
  /* it's not the wiki server! */
  return res;
}

/*----------------------------------------------------------------------
  SafeSaveFileThroughNet
  Send a file through the Network (using the PUT HTTP method) and double
  check for errors using a following GET.
  Return 0 if the file has been saved
  ----------------------------------------------------------------------*/
static int SafeSaveFileThroughNet (Document doc, char *localfile,
                                   char *remotefile, const char *content_type,
                                   ThotBool use_preconditions)
{
  char              msg[MAX_LENGTH];
  char              tempfile[MAX_LENGTH]; /* File name used to refetch */
  char              tempURL[MAX_LENGTH];  /* May be redirected */
  int               res;
  int               mode = 0;
#ifdef AMAYA_DEBUG
  unsigned long     file_size = 0;
#endif
  ThotBool          verify_publish;

  res = SaveWikiFile (doc, localfile, remotefile, content_type,use_preconditions);
  if (res != 2)
    return res;
  TtaGetEnvBoolean ("VERIFY_PUBLISH", &verify_publish);
#ifdef AMAYA_DEBUG
  fprintf(stderr, "Save %s to %s type=%s", localfile, remotefile, content_type);
  AM_GetFileSize (localfile, &file_size);
  fprintf(stderr, " size=%lu\n", file_size);
#endif

  /* Save */
  /* JK: SYNC requests assume that the remotefile name is a static array */
  strcpy (tempfile, remotefile);
  mode = AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST;
  if (use_preconditions)
  mode = mode | AMAYA_USE_PRECONDITIONS;
  res = PutObjectWWW (doc, localfile, tempfile, content_type, NULL,
                      mode, NULL, NULL);
  if (res != 0)
    /* The HTTP PUT method failed ! */
    return (res);
  /* does the user want to verify the PUT? */
  if (!verify_publish)
    return (0);

  /* Refetch */
#ifdef AMAYA_DEBUG
  fprintf(stderr, "Refetch %s \n", remotefile);
#endif

  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), NULL);
  strcpy (tempURL, remotefile);
  res = GetObjectWWW (doc, 0, tempURL, NULL, tempfile, AMAYA_SYNC | AMAYA_NOCACHE
                      | AMAYA_FLUSH_REQUEST, NULL, NULL, NULL, NULL, NO, NULL);
  if (res != 0)
    {
      /* The HTTP GET method failed ! */
      sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED), remotefile);
      InitConfirm (doc, 1, msg);
      if (UserAnswer)
        /* Ignore the read failure */
        res = 0;
    }

  if (res == 0)
    {
      /* Compare content. */
#ifdef AMAYA_DEBUG
      fprintf(stderr, "Compare %s and %s \n", remotefile, localfile);
#endif
      if (! TtaCompareFiles(tempfile, localfile))
        {
          sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED), remotefile);
          InitConfirm (doc, 1, msg);
          if (!UserAnswer)
            res = -1;
        }
    }

  if (TtaFileExist (tempfile))
    TtaFileUnlink (tempfile);
  return(res);
}

/*----------------------------------------------------------------------
  SaveObjectThroughNet
  Save a simple file to a remote network location.
  confirm = TRUE form SAVE_AS and FALSE from SAVE
  ----------------------------------------------------------------------*/
static ThotBool SaveObjectThroughNet (Document doc, View view,
                                      char *url, ThotBool confirm,
                                      ThotBool use_preconditions)
{
  char            *tempname;
  char            *msg;
  const char      *content_type;
  int              remainder = 500;
  int              res;

  msg = (char *)TtaGetMemory (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (doc, url);

  /* build the output */
  if (DocumentTypes[doc] == docSource)
    /* it's a source file, renumber lines */
    TtaExportDocumentWithNewLineNumbers (doc, tempname, "TextFileT", FALSE);
  else
    TtaExportDocument (doc, tempname, "TextFileT");

  ActiveTransfer (doc);
  TtaHandlePendingEvents ();
  
  if (DocumentMeta[doc])
    content_type = DocumentMeta[doc]->content_type;
  else
    content_type = NULL;

  res = SafeSaveFileThroughNet (doc, tempname, url, content_type,
                                use_preconditions);
  if (res != 0)
    {
      DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
      ResetStop (doc);
          InitInfo ("", AmayaLastHTTPErrorMsg);
          res = -1;
#ifdef IV
      sprintf (msg, "%s %s",
               TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
               url);
      if (confirm)
        {
          InitConfirm3L (doc, view, msg, AmayaLastHTTPErrorMsg, 
                         TtaGetMessage (AMAYA, AM_SAVE_DISK), TRUE);
          if (UserAnswer)
            res = -1;
          else
            res = 0;
        }
      else
        {
          InitInfo ("", AmayaLastHTTPErrorMsg);
          res = -1;
        }
      /* erase the last status message */
      TtaSetStatus (doc, view, "", NULL);	  
#endif     
    }
  else
    {
      ResetStop (doc);
#ifdef AMAYA_DEBUG
      fprintf(stderr, "Saving completed\n");
#endif
    }
  TtaFreeMemory (tempname);
  TtaFreeMemory (msg);
  return (res == 0);
}

/*----------------------------------------------------------------------
  SaveLocalCopy saves a copy of the remote file.
  The parameter url is the distant location.
  The parameter tempname points to the local file.
  When the parameter ask is TRUE, a confirmation is requested
  Return 0 if okay, -1 otherwise
  ----------------------------------------------------------------------*/
static int SaveLocalCopy (Document doc, View view, char *url,
                          char *tempname, ThotBool ask)
{
  char                pathname[MAX_LENGTH], *ptr, *last = NULL;
  char                msg[MAX_LENGTH];
  char               *docdir;
  int                 res = -1;

  if (url && !strncmp (url, "http://", 7))
    {
      ptr = strstr (url, "?");
      if (ptr == NULL)
        ptr = strstr (url, "#");
      if (ptr == NULL)
        {
          docdir = TtaGetDocumentsDir ();
          if (DocumentMeta[doc] && DocumentMeta[doc]->full_content_location)
            // use the full name
          sprintf (pathname, "%s%c%s", docdir, DIR_SEP,
                   &(DocumentMeta[doc]->full_content_location[7]));
          else
            sprintf (pathname, "%s%c%s", docdir, DIR_SEP,&url[7]);
          ptr = strstr (pathname, "/");
          while (ptr)
            {
              last = ptr;
              ptr = strstr (&last[1], "/");              
#ifdef _WINDOWS
              *last = DIR_SEP;
#endif /* _WINDOWS */
            }
          if (last[1] == EOS && DocumentMeta[doc] &&
              DocumentMeta[doc]->content_location)
              // add the default name
            strcat (pathname, DocumentMeta[doc]->content_location);
         if (ask)
            {
              sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
	      strcat (msg, "(");
	      strcat (msg, AmayaLastHTTPErrorMsg);
	      strcat (msg, ")");
              InitConfirm3L (doc, 1, msg, TtaGetMessage (AMAYA, AM_SAVING_FAILED),
                             pathname, TRUE);
            }
          else
            UserAnswer = TRUE;
          // create the subdirectory if needed
          if (UserAnswer && last)
            {
              *last = EOS;
              if (TtaCheckMakeDirectory (pathname, TRUE))
                res = 0;
              *last = DIR_SEP;
            }
          // save a copy of the document file
          if (res == 0 && TtaFileCopy (tempname, pathname))
            {
              /* switch Amaya buttons and menus */
              DocStatusUpdate (doc, FALSE);
              TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), pathname);
              /* Notify the document as modified */
              TtaSetDocumentModified (doc);
            }
        }
    }
  return res;
}

/*----------------------------------------------------------------------
  SaveDocumentThroughNet
  Save a document and the included images to a remote network location.
  confirm = TRUE form SAVE_AS and FALSE from SAVE
  ----------------------------------------------------------------------*/
static ThotBool SaveDocumentThroughNet (Document doc, View view, char *url,
                                        ThotBool confirm, ThotBool with_images,
                                        ThotBool use_preconditions)
{
  LoadedImageDesc *pImage;
  char            *tempname;
  char            *msg;
  const char      *content_type;
  int              remainder = 10000;
  int              index = 0, len, nb = 0;
  int              i, res;
  ThotBool        *imgToSave = NULL, savecopy = FALSE;
  ThotBool         manage_images = TRUE;

  msg = (char *)TtaGetMemory (remainder);
  if (msg == NULL)
    return (FALSE);

  /* save into the temporary document file */
  tempname = GetLocalPath (doc, url);

  /* First step : generate the output file and ask for confirmation */
  SetNamespacesAndDTD (doc, RemoveTemplate);
  if (DocumentTypes[doc] == docHTML)
    if (SaveAsXML)
      {
        if (TtaGetDocumentProfile(doc) == L_Xhtml11 || TtaGetDocumentProfile(doc) == L_Basic)
          TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT11", RemoveTemplate);
        else
          TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLTX", RemoveTemplate);
        DocumentMeta[doc]->xmlformat = TRUE;
      }
    else
      {
        TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT", RemoveTemplate);
        DocumentMeta[doc]->xmlformat = FALSE;
      }
  else if (DocumentTypes[doc] == docSVG)
    TtaExportDocumentWithNewLineNumbers (doc, tempname, "SVGT", RemoveTemplate);
  else if (DocumentTypes[doc] == docMath)
    TtaExportDocumentWithNewLineNumbers (doc, tempname, "MathMLT", RemoveTemplate);
  else if (DocumentTypes[doc] == docTemplate)
    TtaExportDocumentWithNewLineNumbers (doc, tempname, "MathMLT", FALSE);
  else if (DocumentTypes[doc] == docXml)
    TtaExportDocumentWithNewLineNumbers (doc, tempname, NULL, RemoveTemplate);
#ifdef BOOKMARKS
  else if (DocumentTypes[doc] == docBookmark)
    BM_DocSave (doc, tempname);
#endif /* BOOKMARKS */
  else if (DocumentTypes[doc] == docImage)
    {
      /* export the new container using the image file name */
      TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLTX", FALSE);
      TtaFreeMemory (tempname);
      pImage = SearchLoadedImageByURL (doc, url);
      tempname = TtaStrdup (pImage->localName);
    }
  res = 0;
  msg[0] = EOS;
  len = 0;
  pImage = ImageURLs;
  if (pImage == NULL)
    {
      manage_images = FALSE;
      pImage = LoadedResources;
    }
  while (pImage)
    {
      if (pImage->document == doc && pImage->status == RESOURCE_MODIFIED)
        {
          if (nb > 30)
            {
              strcpy (&msg[index], "...");
              len = strlen ("...");
              len++;
              remainder -= len;
              index += len;
              nb++;
              break;
            }
          strcpy (&msg[index], pImage->originalName);
          len = strlen (pImage->originalName);
          len++;
          remainder -= len;
          index += len;
          nb++;
        }
      pImage = pImage->nextImage;
      // next list
      if (pImage == NULL && manage_images)
        {
          manage_images = FALSE;
          pImage = LoadedResources;
        }
    }

  if (msg[0] != EOS)
    {
      /* there is almost an image to be saved */
      imgToSave = (ThotBool *)TtaGetMemory (nb * sizeof (ThotBool));
      for (i = 0; i < nb; i++)
        imgToSave[i] = TRUE;

      /* TODO: display the list of saved images */
      ThotBool created =  CreateCheckedListDlgWX( BaseDialog + ConfirmSave,
                                                  TtaGetViewFrame (doc, view),
                                                  TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE),
                                                  nb, msg,
                                                  &imgToSave[0]);
      if (created)
        {
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + ConfirmSave, FALSE, TRUE);
          /* wait for an answer */
          TtaWaitShowDialogue ();
        }
      if (!UserAnswer)
        /* do not continue */
        res = -1;
      else if (ImageURLs)
        {
          pImage = ImageURLs;
          manage_images = TRUE;
        }
      else
        {
          pImage = LoadedResources;
          manage_images = FALSE;
        }
    }
  else
    {
      /* only the document is saved */
      res = 0;
      pImage = NULL;
    }

  /*
   * Second step : saving the HTML content and the images modified locally.
   *               if saving failed, suggest to save to disk.
   */
  if (res == 0)
    {
      ActiveTransfer (doc);
      TtaHandlePendingEvents ();
      if (DocumentMeta[doc])
        content_type = DocumentMeta[doc]->content_type;
      else
        content_type = NULL;
      res = SafeSaveFileThroughNet (doc, tempname, url, content_type, use_preconditions);
      if (res != 0)
        {
          ResetStop (doc);
          TtaHandlePendingEvents ();
          savecopy = TRUE;
          if (savecopy)
            // save a local copy
            res = SaveLocalCopy (doc, view, url, tempname, TRUE);
          else
            {
              char err_msg[MAX_LENGTH];
              DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
              sprintf (err_msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
              ConfirmError3L (doc, 1, err_msg,
                              NULL,
                              AmayaLastHTTPErrorMsg, NULL, FALSE);
              res = -1;
            }
        }

      if (res == 0)
        {
          i = 0;
          while (pImage)
            {
              if (pImage->document == doc && pImage->status == RESOURCE_MODIFIED)
                {
                  if (imgToSave[i++])
                    {
                      if (savecopy)
                        // save a local copy
                        res = SaveLocalCopy (doc, view, pImage->originalName,
                                             pImage->tempfile, FALSE);
                      else
                        {
                          /* we get the MIME type of the image. We reuse whatever the
                             server sent if we have it, otherwise, we try to infer it from
                             the image type as discovered by the handler */
                          if (pImage->content_type)
                            content_type = pImage->content_type;
                          else
                            content_type = PicTypeToMIME ((PicType)pImage->imageType);
                          res = SafeSaveFileThroughNet(doc, pImage->tempfile,
                                                       pImage->originalName, content_type,
                                                       use_preconditions);
                        }
                      if (res)
                        {
                          /* message not null if an error is detected */
                          DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
                          ResetStop (doc);
                          sprintf (msg, "%s %s",
                                   TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
                                   pImage->originalName);
                          InitInfo ("", msg);
                          /*res = -1;*/res = 0;
                          /* continue */
                          /*pImage = NULL;*/
                        }
                      else if (!savecopy)
                        pImage->status = RESOURCE_LOADED;
                    }
                }
              
              if (pImage)
                pImage = pImage->nextImage;
              // next list
              if (pImage == NULL && manage_images)
                {
                  manage_images = FALSE;
                  pImage = LoadedResources;
                }
            }
          if (!savecopy)
            // resources are now saved
            RemoveLoadedResources (doc, &LoadedResources);
          // keep URI changes
          ClearSaveAsUpdate (FALSE);
        }
      else
        // restore previous URIs
        ClearSaveAsUpdate (TRUE);
      ResetStop (doc);
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "Saving completed\n");
#endif
  TtaFreeMemory (imgToSave);
  TtaFreeMemory (msg);
  if (tempname)
    TtaFreeMemory (tempname);
  return (res == 0);
}

/*----------------------------------------------------------------------
  GetDocFromSource
  If sourceDoc is a source document, return the corresponding
  structured document.
  ----------------------------------------------------------------------*/
Document GetDocFromSource (Document sourceDoc)
{
  Document	xmlDoc;
  int		i;

  xmlDoc = 0;
  if (DocumentTypes[sourceDoc] == docSource)
    /* It's a source document */
    for (i = 1; i < DocumentTableLength && xmlDoc == 0; i++)
      if (IsXMLDocType (i))
        if (DocumentSource[i] == sourceDoc)
          xmlDoc = i;
  return xmlDoc;
}


/*----------------------------------------------------------------------
  DoSynchronize
  save the current view (source/structure) in a temporary file
  and update the other view (structure/source).
  ----------------------------------------------------------------------*/
void DoSynchronize (Document doc, View view, NotifyElement *event)
{
  Element           el;
  DisplayMode       dispMode;
  Document          otherDoc;
  char             *tempdoc = NULL;
  char              docname[MAX_LENGTH];
  char              tempdir[MAX_LENGTH];
  int               line, index, method = 0;
  int		            position, distance;
  ThotBool          saveBefore, modified;

  if (!DocumentURLs[doc])
    /* the document is not loaded yet */
    return;

  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docSVG ||
      DocumentTypes[doc] == docLibrary ||
      DocumentTypes[doc] == docMath)
    /* it's a structured document */
    otherDoc = DocumentSource[doc];
  else if (DocumentTypes[doc] == docSource)
    {
      otherDoc = GetDocFromSource (doc);
      if (DocumentMeta[otherDoc])
      method = DocumentMeta[otherDoc]->method;
    }
  else
    otherDoc = 0;

  saveBefore = TtaIsDocumentUpdated (doc);
  modified = TtaIsDocumentModified (doc);
  if (DocumentTypes[doc] == docCSS)
    {
      if (!modified && !saveBefore)
        return;
    }
  else if (!saveBefore)
    /* nothing new to be saved in this view of the document. Let see if
       the other view has been modified */
    {
      if (otherDoc && TtaIsDocumentUpdated (otherDoc))
        /* the other view has not been modified either */
        saveBefore = TRUE;
    }

  /* Only one synchronize at the same time */
  if (Synchronizing)
    return;
  Synchronizing = TRUE;

  /* change display mode to avoid flicker due to callbacks executed when
     saving some elements, for instance META */
  dispMode = TtaGetDisplayMode (doc);
  if (IsXMLDocType (doc))
    /* it's the structured form of the document */
    {
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
       
      tempdoc = GetLocalPath (doc, DocumentURLs[doc]);
      if (saveBefore)
        {
          /* save the current state of the document into the temporary file */
          SetNamespacesAndDTD (doc, FALSE);
          if (DocumentTypes[doc] == docLibrary || DocumentTypes[doc] == docHTML)
            {
              if (TtaGetDocumentProfile (doc) == L_Xhtml11 || TtaGetDocumentProfile (doc) == L_Basic)
                TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "HTMLT11", FALSE);
              else if (DocumentMeta[doc]->xmlformat)
                TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "HTMLTX", FALSE);
              else
                TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "HTMLT", FALSE);
            }
          else if (DocumentTypes[doc] == docSVG)
            TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "SVGT", FALSE);
          else if (DocumentTypes[doc] == docMath)
            TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "MathMLT", FALSE);
          else
            TtaExportDocumentWithNewLineNumbers (doc, tempdoc, NULL, FALSE);
          ResetHighlightedElement ();
          RedisplaySourceFile (doc);
          otherDoc = DocumentSource[doc];
          /* the other document is now different from the original file. It can
             be saved */
          if (modified)
            {
              TtaSetDocumentModified (otherDoc);
              // but updated
              TtaSetDocumentUnupdated (otherDoc);
            }
#ifdef ANNOTATIONS
          ANNOT_Reload (otherDoc, 1);
#endif /* ANNOTATIONS */
        }

      /* restore original display mode */
      TtaSetDisplayMode (doc, dispMode);
    }
  else if (DocumentTypes[doc] == docSource)
    /* it's a source document */
    {
      if (saveBefore && otherDoc)
        {
          /* get the current position in the document */
          position = RelativePosition (otherDoc, &distance);
          TtaClearUndoHistory (otherDoc);
          // By default remove the read-only mode
          TtaSetDocumentAccessMode (otherDoc, 1);
          /* save the current state of the document into the temporary file */
          tempdoc = GetLocalPath (otherDoc, DocumentURLs[otherDoc]);
          TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "TextFileT", FALSE);
          TtaExtractName (tempdoc, tempdir, docname);
          ResetHighlightedElement ();
          RestartParser (otherDoc, tempdoc, tempdir, docname, TRUE, TRUE);
          /* the other document is now different from the original file. It can
             be saved */
          if (modified)
            TtaSetDocumentModified (otherDoc);
          // check if it's a template instance
#ifdef TEMPLATES
          // Fill template internal structures and prepare the instance if any
          Template_FillFromDocument (otherDoc);
#endif /* TEMPLATES */
          /* restore original display mode */
          TtaSetDisplayMode (doc, dispMode);
          /* restore the current position in the document */
          el = ElementAtPosition (otherDoc, position);
          TtaShowElement (otherDoc, 1, el, distance);
          /* but it could be saved too */
          TtaSetItemOn (doc, 1, File, BSave);
          TtaSetItemOn (doc, 1, File, BSaveAll);
        }
    }
  else
    {
      if (saveBefore)
        {
          /* save the current selection */
          GetCurrentLine (doc, &line, &index);
          /* save the current state of the CSS document into the temporary file */
          tempdoc = GetLocalPath (doc, DocumentURLs[doc]);
          TtaExportDocument (doc, tempdoc, "TextFileT");
          TtaExtractName (tempdoc, tempdir, docname);
          RemoveParsingErrors (doc);
          /* reapply the CSS to relative documents */
          UpdateStyleSheet (DocumentURLs[doc], tempdoc);
          if (otherDoc)
            {
            if (modified)
              TtaSetDocumentModified (otherDoc);
            else
              /* the source can be closed without save */
              TtaSetDocumentUnmodified (otherDoc);
            }
        }
    }

  /* disable the Synchronize command for both documents */
  if (DocumentTypes[doc] == docCSS)
    {
      TtaSetItemOff (doc, 1, File, BSynchro);
      TtaSetDocumentUnupdated (doc);
    }
  if (otherDoc)
    {
      // keep the method of the synchornized document
      if (DocumentMeta[otherDoc])
        DocumentMeta[otherDoc]->method = method;
      if (method == CE_TEMPLATE && DocumentTypes[otherDoc] == docHTML)
        // avoid positionned boxes to overlap the xt:head section
        SetBodyAbsolutePosition (otherDoc);
      if (DocumentTypes[doc] != docCSS)
        {
          TtaSetItemOff (otherDoc, 1, File, BSynchro);
          /* updates are now done */
          TtaSetDocumentUnupdated (otherDoc);
        }
      TtaSetItemOff (doc, 1, File, BSynchro);
      /* updates are now done */
      TtaSetDocumentUnupdated (doc);
       
      /* Synchronize selections */
      if (saveBefore)
        {
        /* recheck the clicked element */
        TtaGetClickedElement (&(event->document), &(event->element));
        }
      SynchronizeSourceView (event);
    }
  
  TtaFreeMemory (tempdoc);
  Synchronizing = FALSE;
}

/*----------------------------------------------------------------------
  SaveBefore
  Returns TRUE if the temporary file doesn't include all current changes.
  ----------------------------------------------------------------------*/
ThotBool SaveBefore (Document doc)
{
  Document          otherDoc;

  if (doc == 0 || !DocumentURLs[doc])
    /* the document is not loaded yet */
    return FALSE;
  if (DocumentTypes[doc] == docCSS && !TtaIsDocumentModified (doc))
    return FALSE;
  else if (DocumentTypes[doc] == docHTML ||
           DocumentTypes[doc] == docSVG ||
           DocumentTypes[doc] == docLibrary ||
           DocumentTypes[doc] == docMath)
    /* it's a structured document */
    otherDoc = DocumentSource[doc];
  else if (DocumentTypes[doc] == docSource)
    otherDoc = GetDocFromSource (doc);
  else
    otherDoc = 0;

  if (TtaIsDocumentUpdated (doc) ||
      (otherDoc && TtaIsDocumentUpdated (otherDoc)))
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  Synchronize
  save the current view (source/structure) in a temporary file 
  and update the other view (structure/source).      
  ----------------------------------------------------------------------*/
void Synchronize (Document doc, View view)
{
  NotifyElement     event;

  event.document = doc;
  event.element = NULL;
  DoSynchronize (doc, view, &event);
}

/*----------------------------------------------------------------------
  RedisplayDoc
  Synchronize the document or just redisplay the formatted view.      
  ----------------------------------------------------------------------*/
void RedisplayDoc (Document doc)
{
  char             *tempdoc = NULL;
  char              docname[MAX_LENGTH];
  char              tempdir[MAX_LENGTH];

  if (SaveBefore (doc))
    Synchronize (doc, 1);
  else
    {
      /* do not restart parser if the document didn't change because
         it will lost the selection position */
      tempdoc = GetLocalPath (doc, DocumentURLs[doc]);
      TtaExtractName (tempdoc, tempdir, docname);
      RestartParser (doc, tempdoc, tempdir, docname, TRUE, FALSE);
      TtaFreeMemory (tempdoc);
    }
}


/*----------------------------------------------------------------------
  SaveAll
  Entry point called when the user selects the SaveAll menu entry or
  presses the SaveAll button.
  ----------------------------------------------------------------------*/
void SaveAll (Document doc, View view)
{
  int               document;

  if (Saving_All_lock)
    return;
  Saving_All_lock = TRUE;
  
  /* In case we SaveAll from the modified source and the formatted doc is already modified */
  Synchronize (doc, view);
  for (document = 1; document < DocumentTableLength; document++)
    {
      if (TtaIsDocumentModified (document) &&
          DocumentTypes[document] != docTemplate)
        SaveDocument (document, view);
    }

  Saving_All_lock = FALSE;
#ifdef _WX
  TtaSetItemOff (doc, 1, File, BSaveAll);
#endif /* _WX */
}


/*----------------------------------------------------------------------
  SaveDocument
  Entry point called when the user selects the Save menu entry or
  presses the Save button.
  ----------------------------------------------------------------------*/
void SaveDocument (Document doc, View view)
{
  Document	      xmlDoc;
  DisplayMode         dispMode;
  char                tempname[MAX_LENGTH];
  char                msg[MAX_LENGTH];
  char               *ptr;
  int                 i;
  ThotBool            ok, newLineNumbers;
  ThotBool            with_suffix = FALSE;

  if (DocumentURLs[doc] == 0)
    return;
  if (Saving_lock)
    // there is a current saving operation
    return;

  if (DocumentTypes[doc] == docAnnot) 
    {
#ifdef ANNOTATIONS
      ANNOT_SaveDocument (doc, view);
#endif /* ANNOTATIONS */
      return;
    }

  if (SavingDocument || SavingObject)
    // close the previous saving dialog
    CallbackDialogue (BaseDialog + SaveForm, INTEGER_DATA, (char*) 0);

  TextFormat = (DocumentTypes[doc] == docText ||
                DocumentTypes[doc] == docCSS ||
                DocumentTypes[doc] == docSource);
  /* if it's a source document, get the corresponding HTML document */
  if (DocumentTypes[doc] == docSource)
    xmlDoc = GetDocFromSource (doc);
  else
    xmlDoc = 0;
  if (!TtaIsDocumentModified (doc) &&
      (xmlDoc == 0 || !TtaIsDocumentModified (xmlDoc)))
    {
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NOTHING_TO_SAVE), "");
      Saving_lock = FALSE;
      return;
    }
  SavingDocument = doc;
  RemoveTemplate = FALSE; // don't remove template elements
  ok = FALSE;
  /* attempt to save through network if possible */
  strcpy (tempname, DocumentURLs[doc]);
  /* don't save files that were originally compressed (unless we know how
     to compress them again */
  i = strlen (tempname) - 1;
  if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
    {
      /* add a compress warning */
      return;
      /*tempname[i-2] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaStrdup (tempname);*/
    }
  else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
    {
      /* add a compress warning */
       return;
      /*tempname[i-1] = EOS;
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaStrdup (tempname);*/
    }

#ifdef AMAYA_DEBUG
  fprintf(stderr, "SaveDocument : %d to %s\n", doc, tempname);
#endif

  // start the saving operation
  Saving_lock = TRUE;
  /* change display mode to avoid flicker due to callbacks executed when
     saving some elements, for instance META */
  dispMode = TtaGetDisplayMode (doc);
  if (TextFormat)
    TtaSetDisplayMode (doc, DeferredDisplay);
  else if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* the suffix determines the output format */
  SaveAsXML = IsXMLName (tempname) || DocumentMeta[doc]->xmlformat;
  /* We automatically save a HTML document as a XML one 
     when we have a xhtml profile */
  if (!SaveAsXML &&
      (TtaGetDocumentProfile(doc) == L_Basic ||
       TtaGetDocumentProfile(doc) == L_Xhtml11))
    SaveAsXML = TRUE;
  /* the document will be exported with line numbers */
  newLineNumbers = (DocumentTypes[doc] != docText &&
                    DocumentTypes[doc] != docCSS &&
                    DocumentTypes[doc] != docLog &&
                    DocumentTypes[doc] != docImage);
  if (IsW3Path (tempname))
    /* it's a remote document */
    {
      Synchronize (doc, view);
      if (SavingDocument != doc)
        {
          ok = FALSE;
          // reset the source document as updated
          TtaSetDocumentUpdated (doc);
        }
      else
        ok = HasSavingName (doc, view, tempname, &with_suffix);

      if (!ok || !with_suffix)
        {
          // if the content_location is known, don't redirect to the Save As dialog
          if (!DocumentMeta[doc] || !DocumentMeta[doc]->full_content_location)
            {
              // call Save As when there is no suffix
              SavingDocument = 0;
              Saving_lock = FALSE;
              TtaSetDisplayMode (doc, dispMode);
              SaveDocumentAs (doc, view);
              return;
            }
        }

      ptr = GetLocalPath (doc, DocumentURLs[doc]);
      /*  no need to protect against a null ptr, as GetLocalPath
          will always return something at this point */
      TtaFreeMemory (ptr);
      /* it's a complete name: save it */
      if (ok)
        {
          if (DocumentMeta[doc] && DocumentMeta[doc]->full_content_location)
            ptr = DocumentMeta[doc]->full_content_location;
          else
            ptr = DocumentURLs[doc];
          if (TextFormat)
            ok = SaveObjectThroughNet (doc, view, ptr, FALSE, FALSE);
          else
            ok = SaveDocumentThroughNet (doc, view, ptr, TRUE, TRUE, FALSE);
        }
    }
  else
    {
      /* it's a local document */
      if (DocumentTypes[doc] == docBookmark)
        {
#ifdef BOOKMARKS
          ok = BM_DocSave (doc, tempname);
#else
          ok = FALSE;
#endif /* BOOKMARKS */
        }
      else
        {
          Synchronize (doc, view);
          if (SavingDocument == doc)
            {
              /* save a local copy of the current document */
              if (xmlDoc)
                ptr = GetLocalPath (xmlDoc, tempname);
              else
                ptr = GetLocalPath (doc, tempname);
              ok = TtaFileCopy (ptr, tempname);
              TtaFreeMemory (ptr);
            }
        }
    }

  /* restore original display mode */
  TtaSetDisplayMode (doc, dispMode);

  Saving_lock = FALSE;
  if (SavingDocument == 0)
    // the saving was discarded
    return;
  SavingDocument = 0;
  if (ok)
    {
      /* cancel the possible don't replace mark */
      DontReplaceOldDoc = FALSE;
      if (DocumentMeta[doc] && DocumentMeta[doc]->method == CE_TEMPLATE)
        {
          DocumentMeta[doc]->method = CE_ABSOLUTE;
          DocumentMetaClear (DocumentMeta[doc]);
        }
      /* the document is now saved */
      TtaSetDocumentUnmodified (doc);
      TtaSetInitialSequence (doc);
      if (xmlDoc)
        {
          TtaSetDocumentUnmodified (xmlDoc);
          TtaSetInitialSequence (xmlDoc);
          // Reinitialize the template description
          if (IsTemplateDocument (xmlDoc) && !IsTemplateInstanceDocument(xmlDoc))
            Template_FillFromDocument (xmlDoc);
        }
      else if (DocumentSource[doc])
        {
          // Reinitialize the template description
          if (IsTemplateDocument (doc) && !IsTemplateInstanceDocument(doc))
            Template_FillFromDocument (doc);
          TtaSetDocumentUnmodified (DocumentSource[doc]);
          TtaSetInitialSequence (DocumentSource[doc]);
        }
        
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, FALSE);
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[doc]);

      /* If 'SaveDocument' has not been triggered by a crash,
         remove the auto-saved file that corresponds to the document */
      /* Check if Amaya has crashed */
      sprintf (tempname, "%s%cCrash.amaya", TempFileDirectory, DIR_SEP);
      if (!TtaFileExist (tempname))
        RemoveAutoSavedDoc (doc);
    }
  else if (!IsW3Path (tempname))
    {
      /* cannot save */
      sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
      InitInfo ("", msg);
    }
}


/*----------------------------------------------------------------------
  CanReplaceCurrentDocument
  Return TRUE if the document has not been modified
  and if the user agrees to loose the changes he/she has made.
  ----------------------------------------------------------------------*/
ThotBool CanReplaceCurrentDocument (Document doc, View view)
{
  ThotBool	ret, always_replace;

  ret = TRUE;
  if (DocumentTypes[doc] == docLog || DocumentTypes[doc] == docSource)
    ret = TRUE;
  else if (TtaIsDocumentModified (doc) ||
           (!Synchronizing && DocumentSource[doc] &&
            TtaIsDocumentModified (DocumentSource[doc])))
    {
      TtaGetEnvBoolean ("IGNORE_UPDATES", &always_replace);
      if (always_replace)
        {
          /* doesn't check if the document is modified */
          ExtraChoice = TRUE;
          UserAnswer = FALSE;
        }
      else
        {
          ExtraChoice = FALSE;
          UserAnswer = FALSE;
          /* ask if the user wants to save, quit or cancel */
#ifdef _WX
          ConfirmError3L (doc, view, DocumentURLs[doc],
                          TtaGetMessage (AMAYA, AM_DOC_MODIFIED),
                          NULL,
                          TtaGetMessage (AMAYA, AM_DONT_SAVE),
                          TtaGetMessage (LIB, TMSG_BUTTON_SAVE));
#else /* _WX */
          ConfirmError (doc, view, TtaGetMessage (AMAYA, AM_DOC_MODIFIED),
                        TtaGetMessage (AMAYA, AM_DONT_SAVE),
                        TtaGetMessage (LIB, TMSG_BUTTON_SAVE));
#endif /* _WX */
        }
      if (UserAnswer)
        {
          if (DocumentSource[doc] && !TtaIsDocumentUpdated (doc) &&
              TtaIsDocumentUpdated (DocumentSource[doc]))
            // the last updated version is the document source
            SaveDocument (DocumentSource[doc], view);
          else
            SaveDocument (doc, view);
        }
      else if (ExtraChoice)
        {
          TtaSetDocumentUnmodified (doc);
          if (DocumentSource[doc])
            TtaSetDocumentUnmodified (DocumentSource[doc]);
          /* remove the corresponding auto saved doc */
          RemoveAutoSavedDoc (doc);
        }
      else
        ret = FALSE;
    }
  return ret;
}


/*----------------------------------------------------------------------
  BackupAll save all opened documents when the application crashes
  ----------------------------------------------------------------------*/
void BackUpDocs ()
{
  Document        doc;
  FILE           *f;
  char            pathname[MAX_LENGTH];
  char            docname[MAX_LENGTH];
  char           *ptr;
  int             l;

  /* destroy the lock */
#ifdef _WX
  ptr = TtaGetEnvString ("HOME");
  if (ptr)
    {
#ifdef _WINDOWS
      sprintf (pathname, "%s\amaya-check-instance", ptr);
#else /* _WINDOWS */
      sprintf (pathname, "%s/.amaya-check-instance", ptr);
#endif /* _WINDOWS */
      if (TtaFileExist (pathname))
        TtaFileUnlink (pathname);
    }
#endif /* _WX */
  /* check all modified documents */
  f = NULL;
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentURLs[doc] && TtaIsDocumentModified (doc) && doc != W3Loading &&
        (DocumentTypes[doc] != docSource &&
         DocumentTypes[doc] != docLibrary &&
         DocumentTypes[doc] != docLog))
      {
        if (f == NULL)
          {
            /* open the crash file */
            sprintf (pathname, "%s%cCrash.amaya", TempFileDirectory, DIR_SEP);
            f = TtaWriteOpen (pathname);
            if (f == NULL)
              return;
          }

        /* generate the backup file name */
        SavingDocument = 0;
        ptr = DocumentURLs[doc];
        l = strlen (ptr) - 1;
        if (IsW3Path (ptr) &&  ptr[l] == URL_SEP)
          {
            /* it's a directory name */
            ptr[l] = EOS;
            TtaExtractName (DocumentURLs[doc], pathname, docname);
            ptr[l] = URL_SEP;
            l = 0;
          }
        else
          TtaExtractName (DocumentURLs[doc], pathname, docname);
        if (l == 0)
          sprintf (pathname, "%s%c%d%s.html", TempFileDirectory, DIR_SEP, doc, docname);
        else
          sprintf (pathname, "%s%c%d%s", TempFileDirectory, DIR_SEP, doc, docname);

        /* write the backup file */
        DocumentURLs[doc] = TtaStrdup (pathname);
        DocumentSource[doc] = 0;
        SaveDocument (doc, 1);
        /* register the backup file name and the original document name */
        fprintf (f, "\"%s\" \"%s\" %d\n", pathname, ptr, DocumentTypes[doc]);
        TtaFreeMemory (ptr);
      }
  /* now close the crash file */
  if (f != NULL)
    TtaWriteClose (f);
}

/*----------------------------------------------------------------------
  AutoSaveDocument
  Entry point called when the auto-save procedure is triggered
  ----------------------------------------------------------------------*/
static ThotBool AutoSaveDocument (Document doc, View view, char *local_url)
{
  char                tempname[MAX_LENGTH];
  ThotBool            ok;

  ok = FALSE;
  if (DocumentTypes[doc] == docAnnot) 
    return (ok);
  if (Saving_lock)
    // there is a current saving operation
    return (ok);

  TextFormat = (DocumentTypes[doc] == docText ||
                DocumentTypes[doc] == docCSS ||
                DocumentTypes[doc] == docSource);

  strcpy (tempname, local_url);

#ifdef AMAYA_DEBUG
  fprintf(stderr, "AutoSaveDocument : %d to %s\n", doc, tempname);
#endif

  /* the suffix determines the output format */
  SaveAsXML = IsXMLName (tempname) || DocumentMeta[doc]->xmlformat;
  RemoveTemplate = FALSE; // don't remove template elements
  /* We automatically save a HTML document as a XML one 
     when we have a xhtml profile */
  if (!SaveAsXML &&
      (TtaGetDocumentProfile(doc) == L_Basic ||
       TtaGetDocumentProfile(doc) == L_Xhtml11))
    SaveAsXML = TRUE;

  if (TextFormat)
    {
      if (DocumentTypes[doc] == docSource)
        /* it's a source file. renumber lines */
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "TextFileT", FALSE);
      else
        ok = TtaExportDocument (doc, tempname, "TextFileT");
    }
  else
    {
      SetNamespacesAndDTD (doc, FALSE);
      if (DocumentTypes[doc] == docLibrary || DocumentTypes[doc] == docHTML)
        {
          if (SaveAsXML)
            {
              if (TtaGetDocumentProfile(doc) == L_Xhtml11 || TtaGetDocumentProfile(doc) == L_Basic)
                ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
                                                          "HTMLT11", FALSE);
              else
                ok = TtaExportDocumentWithNewLineNumbers (doc, tempname,
                                                          "HTMLTX", FALSE);
              DocumentMeta[doc]->xmlformat = TRUE;
            }
          else
            {
              ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "HTMLT", FALSE);
              DocumentMeta[doc]->xmlformat = FALSE;
            }
        }
      else if (DocumentTypes[doc] == docSVG)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "SVGT", FALSE);
      else if (DocumentTypes[doc] == docMath)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "MathMLT", FALSE);
      else if (DocumentTypes[doc] == docTemplate)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, "TemplateT", FALSE);
      else if (DocumentTypes[doc] == docXml)
        ok = TtaExportDocumentWithNewLineNumbers (doc, tempname, NULL, FALSE);
    }

  return (ok);
}

/*----------------------------------------------------------------------
  RemoveAutoSavedDoc
  Remove the automatic saved file for the document doc
  ----------------------------------------------------------------------*/
void RemoveAutoSavedDoc (Document doc)
{
  char     pathname[MAX_LENGTH * 2];
  char     docname[MAX_LENGTH];
  char    *url, c;
  int      l;

  if (DocumentURLs[doc] == NULL || DocumentTypes[doc] == docLog)
    return;

#ifdef AMAYA_DEBUG
  fprintf (stderr, "\nRemoveAutoSavedDoc - doc %d\n", doc);
#endif
  
  /* Generate the autosaved file name */
  url = (char *)TtaStrdup (DocumentURLs[doc]);
  l = strlen (url) - 1;

  c =  url[l];
  if (c == URL_SEP)
    url[l] = EOS; /* remove the last / */
  TtaExtractName (url, pathname, docname);
  if (c == URL_SEP)
    url[l] = c; /* restore the last / */
  sprintf (pathname, "%s%c%s%d.bak", TempFileDirectory, DIR_SEP, docname, doc);

  /* Remove the autosaved file */
  if (TtaFileExist (pathname))
    TtaFileUnlink (pathname);  
  /* Remove the autosaved file from the AutoSave list */
  RemoveDocFromSaveList (pathname, url, DocumentTypes[doc]);
  TtaFreeMemory (url);
}

/*----------------------------------------------------------------------
  GenerateAutoSavedDoc
  Generate an automatic saved file for the document doc
  ----------------------------------------------------------------------*/
void GenerateAutoSavedDoc (Document doc)
{
  char            pathname[MAX_LENGTH];
  char            tmpname[MAX_LENGTH];
  char            docname[MAX_LENGTH];
  char           *url, c;
  int             l, interval = 0;
  ThotBool        ok;

  if (DocumentURLs[doc] == NULL || DocumentTypes[doc] == docLog)
    return;

  TtaGetEnvInt ("AUTO_SAVE", &interval);
  if (interval == 0 || DocumentURLs[doc] == NULL)
    return;

#ifdef AMAYA_DEBUG
  fprintf (stderr, "\nGenerateAutoSavedDoc - doc %d\n", doc);
#endif

  /* Generate the autosaved file name */
  url = (char *)TtaStrdup (DocumentURLs[doc]);
  l = strlen (url) - 1;
  c =  url[l];
  if (c == URL_SEP)
    url[l] = EOS; /* remove the last / */
  TtaExtractName (url, pathname, docname);
  TtaFreeMemory (url);
  sprintf (pathname, "%s%c%s%d.bak", TempFileDirectory, DIR_SEP, docname, doc);
  sprintf (tmpname, "%s%c%s.tmp", TempFileDirectory, DIR_SEP, docname);
  /* Write the autosaved file */
  ok = AutoSaveDocument (doc, 1, tmpname);
  if (ok)
    {
      if (TtaFileExist (pathname))
        TtaFileUnlink (pathname);
      TtaFileCopy (tmpname, pathname);
      TtaFileUnlink (tmpname);
    }

  /* Register the autosaved file infos into the AutoSave list */
  AddDocInSaveList (pathname, DocumentURLs[doc], DocumentTypes[doc]);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool DocumentToSave (NotifyDialog *event)
{
  SaveDocument (event->document, 1);
  return TRUE;	/* prevent Thot from performing normal save operation */
}

/*----------------------------------------------------------------------
  DeleteDocImageContext
  Deletes all the context related to docImage document doc and URL url.
  ----------------------------------------------------------------------*/
static void DeleteDocImageContext (Document doc, char *url)
{
  LoadedImageDesc    *pImage;
  
  /* we should remove this line if when we change the code
     to use an ImageDesc for local images too */
  if (!IsHTTPPath (url))
    return;

  pImage = SearchLoadedImageByURL (doc, url);
  if (!pImage)
    /* nothing to be deleted */
    return;

  /* remove it from the list */
  if (ImageURLs == pImage)
    ImageURLs = pImage->nextImage;
  else
    pImage->prevImage->nextImage = pImage->nextImage;

  /* delete the local copy of the image */
  TtaFileUnlink (pImage->tempfile);

  /* free all associated memory */
  TtaFreeMemory (pImage->originalName);
  TtaFreeMemory (pImage->localName);
  TtaFreeMemory (pImage->tempfile);
  TtaFreeMemory (pImage);
}

/*----------------------------------------------------------------------
  UpdateDocImage
  Changes the HTML container to point to the new URL of the image.
  If pictures are saved locally, make the copy.
  The parameter imgbase gives the relative path of the new image directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
static ThotBool UpdateDocImage (Document doc, ThotBool src_is_local,
                                ThotBool dst_is_local, char *newURL)
{
  Element       el;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  char          *ptr;
  char          *localName;
  char          *internalURL;
  LoadedImageDesc    *pImage;
  Language      lang = 0;

  /* get the URL of the image and the element */
  if (! ImageElement (doc, NULL, &el))
    return FALSE;

  /* change the value of the src attribute */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_SRC;
  attr = TtaGetAttribute (el, attrType);
  TtaRegisterAttributeReplace (attr, el, doc);
  TtaSetAttributeText (attr, newURL, el, doc);

  /* change the title of the document */
  elType.ElSSchema = attrType.AttrSSchema;
  elType.ElTypeNum = HTML_EL_TITLE;
  el = TtaGetRootElement (doc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  if (el)
    {
      el = TtaGetFirstChild (el);
      localName = GetLocalPath (doc, newURL);
      ptr = strrchr (localName, DIR_SEP);
      if (ptr)
        {
          ptr++;
          TtaRegisterElementReplace (el, doc);
          TtaSetTextContent (el, (unsigned char *)ptr, lang, doc);
        }
      TtaFreeMemory (localName);
    }

  /* copy the file to the amaya cache if it's a remote save */
  if (!dst_is_local)
    {
      /* make the special internal URL (used to display the image from
         the container */
      internalURL = (char *)TtaGetMemory (sizeof ("internal:")
                                          + strlen (newURL)
                                          + 1);
      sprintf (internalURL, "internal:%s", newURL);
      
      /* make the local name (we switch the extension to .html) */
      localName = (char *)TtaGetMemory (strlen (newURL)
                                        + sizeof (".html"));
      strcpy (localName, newURL);
      ptr = strrchr (localName, '.');
      if (ptr)
        strcpy (ptr, ".html");
      else
        strcat (localName, ".html");
      ptr = GetLocalPath (doc, localName);
      TtaFreeMemory (localName);
      localName = ptr;

      /* create the pImage descriptor if it doesn't exist
         and copy the file */

      pImage = SearchLoadedImageByURL (doc, newURL);
      if (pImage)
        {
          /* copy the old file to the new location */
          TtaFileCopy (pImage->localName, localName);
          if (pImage->originalName != NULL)
            TtaFreeMemory (pImage->originalName);
          pImage->originalName = internalURL;
          if (pImage->localName)
            TtaFreeMemory (pImage->localName);
          pImage->localName = localName;
          pImage->status = RESOURCE_LOADED;
        }
      else
        {
          if (IsHTTPPath (DocumentURLs[doc]))
            {
              /* remote to remote copy */
              pImage = SearchLoadedImageByURL (doc, DocumentURLs[doc]);
              TtaFileCopy (pImage->localName, localName);
            }
          else
            /* local to remote copy */
            TtaFileCopy (DocumentURLs[doc], localName);

          pImage = (LoadedImageDesc *) TtaGetMemory (sizeof (LoadedImageDesc));
          memset ((void *) pImage, 0, sizeof (LoadedImageDesc));
          pImage->originalName = internalURL;
          pImage->localName = localName;
          pImage->prevImage = NULL;
          if (ImageURLs)
            {
              ImageURLs->prevImage = pImage;
              pImage->nextImage = ImageURLs;
            }
          else
            pImage->nextImage = NULL;
          ImageURLs = pImage;
          pImage->document = doc;
          pImage->elImage = NULL;
          pImage->imageType = TtaGetPictureType (el);
        }
      pImage->status = RESOURCE_LOADED;
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  CheckCopiedObjects
  if CopyImage is TRUE change all SRC attribute and CSS background images.
  If pictures are saved locally, make the copy
  else add them to the list of remote images to be copied.
  The parameter imgbase gives the relative path of the new image directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
static void CheckCopiedObjects (Document doc, ThotBool src_is_local,
                                ThotBool dst_is_local, char *imgbase, char *newURL)
{
  SSchema             XHTMLSSchema, MathSSchema, SVGSSchema, XLinkSSchema;
  AttributeType       attrType;
  ElementType         elType;
  Attribute           attr;
  Element             el, root, content, svgpic, next;
  LoadedImageDesc    *pImage;
  Language            lang;
  char                tempfile[MAX_LENGTH];
  char                localpath[MAX_LENGTH];
  char                oldpath[MAX_LENGTH];
  char                tempname[MAX_LENGTH];
  char                imgname[MAX_LENGTH];
  char                url[MAX_LENGTH];
  char               *sStyle, *stringStyle, *buf;
  int                 buflen, max, index;
  ThotBool            copyref;

  /* save the old document path to locate existing images */
  strcpy (oldpath, DocumentURLs[doc]);
  buflen = strlen (oldpath) - 1;
  if (oldpath[buflen] ==  '/')
    oldpath[buflen] = EOS;
  /* path to search image descriptors */
  sprintf (localpath, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);
  
  if (CopyImages)
    {
      /* Change all Picture SRC and prepare the saving process */
      /* 
       *                       \   newpath=local |  newpath=remote
       * oldpath                \                |
       * ------------------------|---------------|------------------
       *        | old img=remote | .amaya->file  | update descriptor
       *  local |----------------|---------------|------------------
       *        | old img=local  | file->file    | add descriptor
       * ------------------------|---------------|------------------
       *        | old img=remote | .amaya->file  | update descriptor
       * remote |----------------|---------------|------------------
       *        | old img=local  |   xxxxxxxxxxxxxxxxxxxxxxxxxxxx
       * ------------------------|---------------|------------------
       */

      XHTMLSSchema = TtaGetSSchema ("HTML", doc);
      MathSSchema = TtaGetSSchema ("MathML", doc);
      SVGSSchema = TtaGetSSchema ("SVG", doc);
      XLinkSSchema = TtaGetSSchema ("XLink", doc);
      root = TtaGetMainRoot (doc);
      /* handle style elements */
      elType = TtaGetElementType (root);
      if (elType.ElSSchema == XHTMLSSchema || elType.ElSSchema == SVGSSchema)
        {
          if (elType.ElSSchema == XHTMLSSchema)
            elType.ElTypeNum = HTML_EL_STYLE_;
          else if (elType.ElSSchema == SVGSSchema)
            elType.ElTypeNum = SVG_EL_style__;
          el = TtaSearchTypedElement (elType, SearchInTree, root);
        }
      else
        el = NULL;
      while (el)
        {
          if (elType.ElTypeNum == HTML_EL_STYLE_)
            content = TtaGetFirstChild (el);
          else
            content = NULL;
          if (content)
            {
              buflen = TtaGetTextLength (content) + 1;
              stringStyle = (char *)TtaGetMemory (buflen);
              TtaGiveTextContent (content, (unsigned char *)stringStyle, &buflen, &lang);
              // Save images but no new local import
              sStyle = UpdateCSSURLs (doc, oldpath, newURL, imgbase, stringStyle,
                                      TRUE, FALSE);
              if (sStyle)
                {
                  /* there is almost one change
                     register the modification to be able to undo it */
                  TtaRegisterElementReplace (content, doc);
                  /* save this new style element string */
                  TtaSetTextContent (content, (unsigned char *)sStyle, lang, doc);
                  TtaFreeMemory (sStyle);
                }
              else
                // no change
                TtaFreeMemory (stringStyle);
            }

          // look for another style element
          next = TtaSearchTypedElement (elType, SearchForward, el);
          if (next)
            {
              el = next;
              elType = TtaGetElementType (el);
            }
          else
            el = NULL;
        }

      max = sizeof (SRC_attr_tab) / sizeof (AttSearch);
      for (index = 0; index < max; index++)
        {
          /* fetch a new attrValue */
          attrType.AttrTypeNum = SRC_attr_tab[index].att;
          switch (SRC_attr_tab[index].type)
            {
            case XHTML_TYPE:
              attrType.AttrSSchema = XHTMLSSchema;
              break;
            case MATH_TYPE:
              attrType.AttrSSchema = MathSSchema;
              break;
            case SVG_TYPE:
              attrType.AttrSSchema = SVGSSchema;
              break;
            case XLINK_TYPE:
              attrType.AttrSSchema = XLinkSSchema;
              break;
            default:
              attrType.AttrSSchema = NULL;
            }
          if (attrType.AttrSSchema)
            TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
          else
            el = NULL;

          while (el && attr)
            {
              copyref = TRUE;
              /* look for a SVG_PICTURE_UNIT element */
              if (el && attrType.AttrSSchema == SVGSSchema)
                {
                  elType = TtaGetElementType (el);
                  elType.ElTypeNum = SVG_EL_PICTURE_UNIT;
                  svgpic = TtaSearchTypedElement (elType, SearchInTree, el);
                  if (svgpic == NULL)
                    /* included image found */
                    copyref = FALSE;
                }
              else
                svgpic = NULL;

              if (copyref)
                {
                  elType = TtaGetElementType (el);
                  if ((attrType.AttrTypeNum == HTML_ATTR_Style_ &&
                       attrType.AttrSSchema == XHTMLSSchema) ||
                      (attrType.AttrTypeNum == MathML_ATTR_style_ &&
                       attrType.AttrSSchema == MathSSchema) ||
                      (attrType.AttrTypeNum == SVG_ATTR_style_  &&
                       attrType.AttrSSchema == SVGSSchema))
                    {
                      buflen = TtaGetTextAttributeLength (attr) + 1;
                      stringStyle = (char *)TtaGetMemory (buflen);
                      TtaGiveTextAttributeValue (attr, stringStyle, &buflen);
                      // Save images but no new local import
                      sStyle = UpdateCSSURLs (doc, oldpath, newURL, imgbase, stringStyle,
                                              TRUE, FALSE);
                      if (sStyle)
                        {
                          /* register the modification to be able to undo it */
                          TtaRegisterAttributeReplace (attr, el, doc);
                          /* save this new style attribute string */
                          TtaSetAttributeText (attr, sStyle, el, doc);
                          TtaFreeMemory (sStyle);
                        }
                      else
                        TtaFreeMemory (stringStyle);
                    }
                  else
                    {
                      buflen = MAX_LENGTH;
                      buf = (char *)TtaGetMemory (buflen);
                      buf[0] = EOS;
                      if (buf)
                        {
                          TtaGiveTextAttributeValue (attr, buf, &buflen);
                          /* extract the old image name and location */
                          strcpy (url, buf);
                          NormalizeURL (url, 0, buf, imgname, oldpath);
                          /* save the new SRC attr value */
                          if (imgbase[0] != EOS)
                            {
                              /* compose the relative or absolute name */
                              strcpy (url, imgbase);
                              strcat (url, imgname);
                            }
                          else
                            /* in same directory -> local name */
                            strcpy (url, imgname);
			   
                          NormalizeURL (url, 0, tempname, imgname, newURL);
                          /* register the modification to be able to undo it */
                          TtaRegisterAttributeReplace (attr, el, doc);
                          TtaSetAttributeText (attr, url, el, doc);

                          if (svgpic)
                            /* now work with the SVG picture element */
                            el = svgpic;
                          /*
                            At this point:
                            - url gives the relative new image name
                            - tempname gives the new image full name
                            - buf gives the old image full name
                            - imgname contains the image file name
                          */
                          if (url[0] != EOS && buf[0] != EOS)
                            {
#ifdef AMAYA_DEBUG
                              fprintf(stderr, "     SRC from %s to %s\n", buf, url);
#endif
                              if (src_is_local && !dst_is_local)
                                {
                                  /* add the localfile to the images list */
                                  AddLocalResource (buf, imgname, tempname, doc,
                                                    &pImage, &ImageURLs, FALSE);
                                  /* get image type */
                                  if (pImage)
                                    pImage->imageType = TtaGetPictureType (el);
                                }
                              /* mark the image descriptor or copy the file */
                              if (dst_is_local)
                                {
                                  /* do a file copy */
                                  if (IsW3Path (buf) || IsHTTPPath (oldpath))
                                    {
                                      /*
                                        it was a remote image:
                                        we use the local temporary name to do the copy
                                      */
                                      strcpy (buf, localpath);
                                      strcat (buf, imgname);
                                      pImage = SearchLoadedImage (buf, doc);
                                      if (pImage)
                                        strcpy (buf, pImage->tempfile);
                                    }
			       
                                  if (imgbase[0] != EOS)
                                    strcpy (tempfile, tempname);
                                  else
                                    {
                                      strcpy (tempfile, SavePath);
                                      strcat (tempfile, DIR_STR);
                                      strcat (tempfile, imgname);
                                    }
                                  TtaFileCopy (buf, tempfile);
                                }
                              else
                                {
                                  /* save on a remote server */
                                  if (IsW3Path (buf) || IsHTTPPath (oldpath))
                                    {
                                      /*
                                        it was a remote image:
                                        get the image descriptor to prepare
                                        the saving process
                                      */
                                      strcpy (tempfile, localpath);
                                      strcat (tempfile, imgname);
                                      pImage = SearchLoadedImage (tempfile, doc);
                                      /* update the descriptor */
                                      if (pImage)
                                        {
                                          if (pImage->originalName &&
                                              strcmp (pImage->originalName, tempname))
                                            RegisterSaveAsUpdate (pImage, tempname);
#ifdef IV
                                          /* image was already loaded */
                                            {
                                              TtaFreeMemory (pImage->originalName);
                                              pImage->originalName = TtaStrdup (tempname);
                                              if (TtaFileExist(pImage->tempfile))
                                                pImage->status = RESOURCE_MODIFIED;
                                              else
                                                pImage->status = RESOURCE_NOT_LOADED;
                                            }
#endif
                                        }
                                    }
                                }
                            }
                          TtaFreeMemory (buf);
                        }
                    }
                }
              TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
            }
        }
    }
}


/*----------------------------------------------------------------------
  UpdateCss
  if CopyResources is TRUE change all CSS links
  If Css are saved locally, make the copy
  else add them to the list of remote files to be copied.
  The parameter resbase gives the relative path of the new css directory.
  The parameter newURL gives the new document URL (or local file).
  ----------------------------------------------------------------------*/
static void UpdateCss (Document doc, ThotBool src_is_local,
                       ThotBool dst_is_local, char *resbase, char *newURL)
{
  CSSInfoPtr          css, cssnext, cssnew, cssdone[20];
  PInfoPtr            pInfo, pnext;
  LoadedImageDesc    *desc;
  int                 buflen, index, i;
  CSSmedia            media;
  CSSCategory         category;
  char                oldpath[MAX_LENGTH];
  char                newpath[MAX_LENGTH];
  char                cssname[MAX_LENGTH];
  char                localname[MAX_LENGTH];
  char                url[MAX_LENGTH], *s;
  AttributeType       attrType;
  ElementType         elType;
  Attribute           attr;
  Element             el;

  for (index = 0; index < 20; index++)
    cssdone[index] = NULL;
  index = 0;
  if (resbase[0] != EOS)
    {
      // new path directory
      NormalizeURL (resbase, 0, newpath, cssname, newURL);
    }
  else
    {
      // new path directory
      TtaExtractName (newURL, newpath, cssname);
      /* add the separator if needed */
      buflen = strlen (newpath) - 1;
      if (dst_is_local && !IsW3Path (newpath))
        {
          if (newpath[buflen] != DIR_SEP)
            strcat (newpath, DIR_STR);
        }
      else
        {
          if (newpath[buflen] != URL_SEP)
            strcat (newpath, URL_STR);
        }
    }
  
  css = CSSList;
  /* count the number of menu entries */
  while (css)
    {
      cssnext = css->NextCSS;
      pInfo = css->infos[doc];
      while (pInfo)
        {
          pnext = pInfo->PiNext;
          if (pInfo &&
              pInfo->PiCategory != CSS_USER_STYLE &&
              pInfo->PiCategory != CSS_DOCUMENT_STYLE)
            {
              /* Change all Picture SRC and prepare the saving process */
              /* 
               *                       \   newpath=local |  newpath=remote
               * oldpath                \                |
               * ------------------------|---------------|------------------
               *        | old css=remote | .amaya->file  | update descriptor
               *  local |----------------|---------------|------------------
               *        | old css=local  | file->file    | add descriptor
               * ------------------------|---------------|------------------
               *        | old css=remote | .amaya->file  | update descriptor
               * remote |----------------|---------------|------------------
               *        | old css=local  |   xxxxxxxxxxxxxxxxxxxxxxxxxxxx
               * ------------------------|---------------|------------------
               */
              oldpath[0] = EOS;
              localname[0] = EOS;
              url[0] = EOS;
              if (css->url)
                strcpy (oldpath, css->url);
              if (css->localName)
                strcpy (localname, css->localName);
              TtaExtractName (oldpath, url, cssname);
              // update @import and images to point to future next resources
              UpdateStyleSheetContent (0, localname, oldpath, newpath);
              strcpy (url, newpath);
              strcat (url, cssname);
              el = pInfo->PiLink;
              media = pInfo->PiMedia;
              category = pInfo->PiCategory;
              // copy the file
              if (localname[0] != EOS)
                {
#ifdef AMAYA_DEBUG
  fprintf(stderr, "UpdateCSS: from %s to %s\n", oldpath, url);
#endif
                  if (dst_is_local)
                    TtaFileCopy (localname, url);
                  else
                    {
                      AddLocalResource (localname, cssname, url, doc,
                                        &desc, &LoadedResources, TRUE);
#ifdef IV
                      ActiveTransfer (doc);
                      TtaHandlePendingEvents ();
                      res = PutObjectWWW (doc, localname, url, "text/css", NULL,
                                          AMAYA_SYNC | AMAYA_NOCACHE | AMAYA_FLUSH_REQUEST,
                                          NULL, NULL);
#endif
                    }
                }
              // update the link
              UnlinkCSS (css, doc, el, TRUE, TRUE, TRUE);
              cssnew = AddCSS (0, doc, category, media, url, localname, el);
              if (index < 20)
                // this css entry is already managed
                cssdone[index++] = cssnew;

              if (el)
                {
                  elType = TtaGetElementType (el);
                  if (elType.ElSSchema)
                    {
                      s = TtaGetSSchemaName (elType.ElSSchema);
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = 0;
                      attr = NULL;
                          // it could be a PI_line
                      if (!strcmp (s, "HTML") && elType.ElTypeNum == HTML_EL_LINK)
                        attrType.AttrTypeNum = HTML_ATTR_HREF_;
                      /* save the new attr value */
                      if (resbase[0] != EOS)
                        {
                          /* compose the relative or absolute name */
                          strcpy (url, resbase);
                          strcat (url, cssname);
                        }
                      else
                        /* in same directory -> local name */
                        strcpy (url, cssname);
                      if (attrType.AttrTypeNum != 0)
                        attr = TtaGetAttribute (el, attrType);
                      if (attr)
                        {
                          /* register the modification to be able to undo it */
                          TtaRegisterAttributeReplace (attr, el, doc);
                          TtaSetAttributeText (attr, url, el, doc);
                        }
                      else
                        /* todo */;
                    }
                }
            }
          pInfo = pnext;
        }
      css = cssnext;
      // check if thi CSS is new generated
      i = 0;
      while (css && i < index)
        {
          if (css == cssdone[i])
            css = css->NextCSS;
          i++;
        }
    }
}


/*----------------------------------------------------------------------
  DoSaveAs
  This function is called when the user press the OK button on the
  Save As dialog. This is tricky, one must take care of a lot of
  parameters, whether initial and final location are local or remote
  and recomputes URLs accordingly.
  When tempSave is TRUE the new document is not registered
  fullCopy is TRUE if local resources must be copied.
  ----------------------------------------------------------------------*/
void DoSaveAs (char *user_charset, char *user_mimetype, ThotBool fullCopy)
{
  NotifyElement       event;
  Document            doc;
  AttributeType       attrType;
  ElementType         elType;
  Element             el, root, doc_url;
  DisplayMode         dispMode;
  CHARSET             charset;
  char               *documentFile;
  char               *tempname, *oldLocal, *newLocal = NULL;
  char               *imagePath = NULL, *cssPath = NULL, *base;
  char                imgbase[MAX_LENGTH], resbase[MAX_LENGTH];
  char                documentname[MAX_LENGTH];
  char                tempdir[MAX_LENGTH];
  char                msg[MAX_LENGTH];
  char                url_sep;
  int                 len, xmlDoc;
  char               *old_charset = NULL;
  char               *old_mimetype = NULL;
  char               *old_content_location = NULL;
  char               *old_full_content_location = NULL;
  char               *ptr;
  int                 buflen;
  ThotBool            src_is_local;
  ThotBool            dst_is_local, ok;
  ThotBool	          docModified, toUndo, with_suffix = FALSE;

  if (SavingDocument == 0)
    return;
  if (Saving_lock)
    // there is a current saving operation
    return;

  src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
  dst_is_local = !IsW3Path (SavePath);
  ok = TRUE;
  toUndo = FALSE;
  base = NULL;

  /* save the context */
  doc = SavingDocument;

#ifdef AMAYA_DEBUG
  fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n",
          DocumentURLs[SavingDocument], SavePath, SaveName, (int) CopyImages);
#endif

  /* New document path */
  documentFile = (char *)TtaGetMemory (MAX_LENGTH);
  strcpy (documentFile, SavePath);
  len = strlen (documentFile);
  if (len > 0 && documentFile [len -1] != DIR_SEP && documentFile [len - 1] != '/')
    {
      if (dst_is_local)
        {
          strcat (documentFile, DIR_STR);
          url_sep = DIR_SEP;
        }
      else
        {
          strcat (documentFile, URL_STR);
          url_sep = URL_SEP;
        }
    }
  else if (dst_is_local)
    url_sep = DIR_SEP;
  else
    url_sep = URL_SEP;

  if (SaveName[0] == EOS)
    {
      /* there is no document name */
      ok = HasSavingName (SavingDocument, 1, documentFile, &with_suffix);
      if (!ok)
        {
          /* cannot save */
          doc = SavingDocument;
          sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
          InitInfo (NULL, msg);
          SavingDocument = 0;
          /* display the dialog box */
          InitSaveForm (doc, 1, documentFile);
        }
    }
  else
    strcat (documentFile, SaveName);

  // start the saving operation
  Saving_lock = TRUE;
  doc = SavingDocument;
  // remove extra '/'
  len = strlen(SaveImgsURL);
  if (len && SaveImgsURL[len-1] == url_sep)
    SaveImgsURL[len-1] = EOS;
  if (!strcmp (SavePath, SaveImgsURL))
    // same path
    SaveImgsURL[0] = EOS;
  if (ok && dst_is_local)
    {
      /* verify that we don't overwite anything and ask for confirmation */
      if (TtaFileExist (documentFile))
        {
          /* ask confirmation */
          tempname = (char *)TtaGetMemory (MAX_LENGTH);
          sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), documentFile);
          InitConfirm (doc, 1, tempname);
          TtaFreeMemory (tempname);
          if (!UserAnswer)
            {
              /* the user has to change the name of the saving file */
              /* display the dialog box */
              Saving_lock = FALSE;
              InitSaveForm (doc, 1, documentFile);
              ok = FALSE;
            }
        }
      /* verify that the directory exists */
      else if (!TtaCheckMakeDirectory (SavePath, TRUE))
        {
          sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), SavePath);
          InitInfo (NULL, msg);
          /* the user has to change the name of the images directory */
          /* display the dialog box */
          SavingDocument = 0;
          Saving_lock = FALSE;
          InitSaveForm (doc, 1, documentFile);
          ok = FALSE;
        }
    }
 
  /* Check information before starting the operation */
  if (ok && !TextFormat)
    {
      /* search if there is a BASE element within the document */
      root = TtaGetMainRoot (doc);
      elType.ElSSchema = TtaGetDocumentSSchema (doc);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          attrType.AttrSSchema = elType.ElSSchema;
          /* search the BASE element */
          elType.ElTypeNum = HTML_EL_BASE;
          el = TtaSearchTypedElement (elType, SearchInTree, root);
          if (el)
            /* URLs are still relative to the document base */
            base = GetBaseURL (doc);
        }
      else
        base = NULL;
      /* Create the base directory/url for the images output */
      imgbase[0] = EOS;
      resbase[0] = EOS;
      if (CopyImages)
        {
          if (SaveImgsURL[0] != EOS)
            {
              if (base)
                imagePath = MakeRelativeURL (SaveImgsURL, base);
              else
                imagePath = MakeRelativeURL (SaveImgsURL, documentFile);
            }
          else if (base)
            imagePath = MakeRelativeURL (SavePath, base);
          if (imagePath)
            {
              strcpy (imgbase, imagePath);
              TtaFreeMemory (imagePath);
              if (imgbase[0] != EOS)
                {
                  /* add the separator if needed */
                  buflen = strlen (imgbase) - 1;
                  if (dst_is_local && !IsW3Path (imgbase))
                    {
                      if (imgbase[buflen] != DIR_SEP)
                        strcat (imgbase, DIR_STR);
                    }
                  else
                    {
                      if (imgbase[buflen] != URL_SEP)
                        strcat (imgbase, URL_STR);
                    }
                }
            }
        }

      if (CopyResources)
        {
          if (SaveCssURL[0] != EOS)
            {
              if (base)
                cssPath = MakeRelativeURL (SaveCssURL, base);
              else
                cssPath = MakeRelativeURL (SaveCssURL, documentFile);
            }
          else
            cssPath = MakeRelativeURL (SavePath, base);
          if (cssPath)
            {
              strcpy (resbase, cssPath);
              TtaFreeMemory (cssPath);
              /* add the separator if needed */
              if (resbase[0] != EOS)
                {
                  buflen = strlen (resbase) - 1;
                  if (dst_is_local && !IsW3Path (resbase))
                    {
                      if (resbase[buflen] != DIR_SEP)
                        strcat (resbase, DIR_STR);
                    }
                  else
                    {
                      if (resbase[buflen] != URL_SEP)
                        strcat (resbase, URL_STR);
                    }
                }
            }
        }

      /* verify that the directory exists */
      if (dst_is_local)
        {
          tempname = (char *)TtaGetMemory (MAX_LENGTH);
          if (imgbase[0] != DIR_SEP)
            {
              strcpy (tempname, SavePath);
              strcat (tempname, DIR_STR);
              strcat (tempname, imgbase);
            }
          else
            strcpy(tempname, imgbase);
          ok = TtaCheckMakeDirectory (tempname, TRUE);
          if (!ok)
            {
              sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempname);
              InitInfo (NULL, msg);
              TtaFreeMemory (tempname);
              /* free base before returning*/
              if (base)
                TtaFreeMemory (base);
              /* the user has to change the name of the images directory */
              /* display the dialog box */
              Saving_lock = FALSE;
              InitSaveForm (doc, 1, documentFile);
            }
          else
            {
              if (resbase[0] != DIR_SEP)
                {
                  strcpy (tempname, SavePath);
                  strcat (tempname, DIR_STR);
                  strcat (tempname, resbase);
                }
              else
                strcpy(tempname, resbase);
              ok = TtaCheckMakeDirectory (tempname, TRUE);
              if (!ok)
                {
                  sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), tempname);
                  InitInfo (NULL, msg);
                  TtaFreeMemory (tempname);
                  /* free base before returning*/
                  if (base)
                    TtaFreeMemory (base);
                  /* the user has to change the name of the images directory */
                  /* display the dialog box */
                  Saving_lock = FALSE;
                  InitSaveForm (doc, 1, documentFile);
                }
              else
                TtaFreeMemory (tempname);
            }
        }
    }

  /*
    The saving could change URLs and SRC attributes in the document.
    The current state of the document is saved in a backup file
    and can be retored if the operation fails
  */
  if (ok)
    {
      /* cancel the possible don't replace mark */
      DontReplaceOldDoc = FALSE;
      /* the suffix determines the output format */
      docModified = TtaIsDocumentModified (doc);
      /* name of local temporary files */
      oldLocal = GetLocalPath (doc, DocumentURLs[doc]);
      /* adjust the charset and MIME type */
      if (user_charset && *user_charset)
        {
          if (DocumentMeta[doc]->charset)
            old_charset = DocumentMeta[doc]->charset;
          DocumentMeta[doc]->charset = TtaStrdup (user_charset);
          /* change the charset of the doc itself */
          charset = TtaGetCharset (user_charset);
          TtaSetDocumentCharset (doc, charset, FALSE);
        }
      else
        charset = TtaGetDocumentCharset (doc);

      if (user_mimetype && *user_mimetype)
        {
          if (DocumentMeta[doc]->content_type)
            old_mimetype = DocumentMeta[doc]->content_type;
          DocumentMeta[doc]->content_type = TtaStrdup (user_mimetype);
        }
      
      /* save the previous content_location */
      if (DocumentMeta[doc]->content_location) 
        {
          old_content_location = DocumentMeta[doc]->content_location;
          DocumentMeta[doc]->content_location = NULL;
        }
      
      /* save the previous full content_location */
      if (DocumentMeta[doc]->full_content_location) 
        {
          old_full_content_location = DocumentMeta[doc]->full_content_location;
          DocumentMeta[doc]->full_content_location = NULL;
        }
      
      /* change display mode to avoid flicker due to callbacks executed when
         saving some elements, for instance META */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

      if (TextFormat)
        {
          if (dst_is_local)
            /* Local to Local or Remote to Local */
            /* save the local document */
            ok = TtaExportDocument (doc, documentFile, "TextFileT");
          else
            ok = SaveObjectThroughNet (doc, 1, documentFile, TRUE, TRUE);
          if (DocumentTypes[doc] == docCSS)
            /* reapply the CSS to relative documents */
            UpdateStyleSheet (DocumentURLs[doc], documentFile);
        }
      else
        {
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          /* Transform all URLs to absolute ones */
          if (UpdateURLs || CopyResources)
            {
              if (base)
                /* URLs are still relative to the document base */
                SetRelativeURLs (doc, base, resbase,
                                 CopyImages, CopyResources, fullCopy, TRUE);
              else
                /* URLs are relative to the new document directory */
                SetRelativeURLs (doc, documentFile, resbase,
                                 CopyImages, CopyResources, fullCopy,
                                 TRUE);
            }
          /* now free base */
          if (base)
            TtaFreeMemory (base);

          /* Change the document URL and if CopyImage is TRUE change all
           * picture SRC attribute. If pictures are saved locally, make the
           * copy else add them to the list of remote images to be copied.
           */
          if (DocumentTypes[doc] == docImage)
            UpdateDocImage (doc, src_is_local, dst_is_local, documentFile);
          else
            {
              if (CopyImages &&
                  DocumentTypes[doc] != docMath &&
                  DocumentTypes[doc] != docTemplate &&
                  DocumentTypes[doc] != docXml)
                CheckCopiedObjects (doc, src_is_local, dst_is_local, imgbase, documentFile);
              if (CopyResources)
                UpdateCss (doc, src_is_local, dst_is_local, resbase, documentFile);
            }
          toUndo = TtaCloseUndoSequence (doc);
          if (dst_is_local)
            /* Local to Local or Remote to Local */
            /* save the local document */
            ok = SaveDocumentLocally (doc, SavePath, SaveName);
          else
            /* Local to Remote or Remote to Remote */
            /* now save the file as through the normal process of saving */
            /* to a remote URL. */
            ok = SaveDocumentThroughNet (doc, 1, documentFile, TRUE,
                                         CopyImages, TRUE);
        }

      /* restore original display mode */
      TtaSetDisplayMode (doc, dispMode);

      /* the saving operation is finished now */
      SavingDocument = 0;
      Saving_lock = FALSE;
      if (ok && !fullCopy)
        {
          if (toUndo)
            TtaCancelLastRegisteredSequence (doc);
          /* remove the previous docImage context */
          if (DocumentTypes[doc] == docImage)
            DeleteDocImageContext (doc, DocumentURLs[doc]);
          /* add to the history the data of the previous document */
          if (DocumentTypes[doc] == docSource)
            {
              /* the original document must be updated */
              xmlDoc = GetDocFromSource (doc);
              /* update the source document */
              TtaFreeMemory (DocumentURLs[doc]);
              DocumentURLs[doc] = TtaStrdup (documentFile);
              TtaSetDocumentUnmodified (doc);
              TtaSetInitialSequence (doc);
              /* switch Amaya buttons and menus */
              DocStatusUpdate (doc, FALSE);
            }
          else
            xmlDoc = doc;

#ifdef TEMPLATES
          NewXTigerTemplatePath (xmlDoc, documentFile);
#endif /* TEMPLATES */
          AddDocHistory (xmlDoc, DocumentURLs[xmlDoc], 
                         DocumentMeta[xmlDoc]->initial_url,
                         DocumentMeta[xmlDoc]->form_data,
                         DocumentMeta[xmlDoc]->method);
          /* change the document url */
          if (TextFormat || !SaveAsText)
            {
              /*  Remove the auto-saved file that corresponds to the old document */
              RemoveAutoSavedDoc (xmlDoc);
	      
              TtaFreeMemory (DocumentURLs[xmlDoc]);
              DocumentURLs[xmlDoc] = TtaStrdup (documentFile);
              /* Update the Document_URL element */
              root = TtaGetMainRoot (xmlDoc);
              elType = TtaGetElementType (root);
              doc_url = NULL;
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                {
                  elType.ElTypeNum = HTML_EL_Document_URL;
                  doc_url = TtaSearchTypedElement (elType, SearchInTree, root);
                }
              else if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "TextFile"))
                {
                  elType.ElTypeNum = TextFile_EL_Document_URL;
                  doc_url = TtaSearchTypedElement (elType, SearchInTree, root);
                }
              if (doc_url != NULL)
                {
                  doc_url = TtaGetFirstChild (doc_url);
                  TtaSetTextContent (doc_url, (unsigned char *)DocumentURLs[xmlDoc],
                                     TtaGetDefaultLanguage (), xmlDoc);
                }
              AddURLInCombobox (DocumentURLs[xmlDoc], NULL, TRUE);
              TtaSetTextZone (xmlDoc, 1, URL_list);
              if (DocumentSource[doc])
                {
                  TtaFreeMemory (DocumentURLs[DocumentSource[doc]]);
                  DocumentURLs[DocumentSource[doc]] = TtaStrdup (documentFile);
                  /* update content_type and charset */
                  TtaFreeMemory (DocumentMeta[DocumentSource[doc]]->content_type);
                  DocumentMeta[DocumentSource[doc]]->content_type = TtaStrdup (DocumentMeta[doc]->content_type);
                  TtaFreeMemory (DocumentMeta[DocumentSource[doc]]->charset);
                  DocumentMeta[DocumentSource[doc]]->charset = TtaStrdup (DocumentMeta[doc]->charset);
                }
              if (DocumentMeta[xmlDoc]->method == CE_TEMPLATE)
                {
                  DocumentMeta[xmlDoc]->method = CE_ABSOLUTE;
                  DocumentMetaClear (DocumentMeta[xmlDoc]);
                }
              TtaSetDocumentUnmodified (xmlDoc);
              TtaSetInitialSequence (xmlDoc);
	      
              /* switch Amaya buttons and menus */
              DocStatusUpdate (xmlDoc, FALSE);
            }

          if (doc != xmlDoc)
            {
              /* It's a source document. Reparse the corresponding HTML document */
              TtaExtractName (documentFile, tempdir, documentname);
              RestartParser (xmlDoc, documentFile, tempdir, documentname, TRUE, FALSE);
              TtaSetDocumentUnmodified (xmlDoc);
              TtaSetInitialSequence (xmlDoc);
              /* Synchronize selections */
              event.document = doc;
              event.element = NULL;
              SynchronizeSourceView (&event);
            }
          else
            {
              /* if it's a HTML document and the source view is open, redisplay
                 the source. */
              if (DocumentSource[doc])
                // update the source charset
                TtaSetDocumentCharset (DocumentSource[doc], charset, FALSE);
              RedisplaySourceFile (doc);
            }

          /* Sucess of the operation */
          TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_SAVED), documentFile);
          /* remove the previous temporary file */
          newLocal = GetLocalPath (doc, documentFile);
          if (oldLocal && !SaveAsText && strcmp (oldLocal, newLocal))
            /* free the previous temporary file */
            TtaFileUnlink (oldLocal);
          /* free the previous mime type and charset */
          if (old_charset)
            TtaFreeMemory (old_charset);
          if (old_mimetype)
            TtaFreeMemory (old_mimetype);
          if (old_content_location)
            TtaFreeMemory (old_content_location);
          if (old_full_content_location)
            TtaFreeMemory (old_full_content_location);

          // Reinitialize the template description
          Template_FillFromDocument (xmlDoc);
        }
      else
        {
          /*
            Operation failed:
            restore the previous contents of the document
          */
          if (toUndo)
            TtaUndoNoRedo (doc);
          // restore previous URIs
          ClearSaveAsUpdate (TRUE);
          if (!ok)
            {
              sprintf (msg, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), documentFile);
              InitInfo (NULL, msg);
            }
          /* restore the previous status of the document */
          if (DocumentTypes[doc] == docImage)
            {
              DeleteDocImageContext (doc, documentFile);
              /* free the previous temporary file */
              ptr = GetLocalPath (doc, documentFile);
              TtaFileUnlink (ptr);
              TtaFreeMemory (ptr);
            }
          if (!docModified)
            {
              TtaSetDocumentUnmodified (doc);
              TtaSetInitialSequence (doc);
              /* switch Amaya buttons and menus */
              DocStatusUpdate (doc, docModified);
            }
          /* restore the previous charset and mime type */
          if (user_charset && DocumentMeta[doc]->charset)
            {
              TtaFreeMemory (DocumentMeta[doc]->charset);
              DocumentMeta[doc]->charset = old_charset;
              charset = TtaGetCharset (old_charset);
              TtaSetDocumentCharset (doc, charset, FALSE);
            }
          if (user_mimetype && DocumentMeta[doc]->content_type)
            {
              TtaFreeMemory (DocumentMeta[doc]->content_type);
              DocumentMeta[doc]->content_type = old_mimetype;
            }
          if (old_content_location && DocumentMeta[doc]->content_location)
            {
              TtaFreeMemory (DocumentMeta[doc]->content_location);
              DocumentMeta[doc]->content_location = old_content_location;
            }
          if (old_full_content_location && DocumentMeta[doc]->full_content_location)
            {
              TtaFreeMemory (DocumentMeta[doc]->full_content_location);
              DocumentMeta[doc]->full_content_location = old_full_content_location;
            }
          if (!ok)
            /* propose to save a second time */
            SaveDocumentAs(doc, 1);
        }
      TtaFreeMemory (oldLocal);
      TtaFreeMemory (newLocal);
    }
  TtaFreeMemory (documentFile);
}


/*----------------------------------------------------------------------
  SaveTempCopy saves a document to a local temporary directory.
  Saves images and stylesheets.
  filename, if not NULL, will receive a copy of the filename.
  ----------------------------------------------------------------------*/
ThotBool SaveTempCopy (Document doc, const char* dstdir, char** filename)
{
  char buff[MAX_LENGTH];
  char* oldURL;
  char* defaultName;
  
  oldURL = TtaStrdup(DocumentURLs[doc]);
  if (Saving_lock)
    // there is a current saving operation
    return FALSE;

  SavingDocument = doc;
  if (!TtaCheckMakeDirectory ((char*)dstdir, TRUE))
    return FALSE;
  
  SavePath = TtaStrdup(dstdir);
  SaveName = (char *)TtaGetMemory (MAX_LENGTH);
  TtaExtractName (DocumentURLs[doc], buff, SaveName);
  if (SaveName[0] == EOS)
    {
      defaultName = TtaGetEnvString ("DEFAULTNAME");
      if (defaultName == NULL || *defaultName == EOS)
        strcpy(SaveName, StdDefaultName);
      else
        strcpy(SaveName, defaultName);
    }
  
  CopyImages = FALSE;
  CopyResources = FALSE;
  UpdateURLs = TRUE; // just copy local resources
  SaveAsXML = DocumentMeta[doc]->xmlformat;
  SaveAsText = FALSE; // don't generate text from HTML
  RemoveTemplate = FALSE; // don't remove template elements
  TextFormat = (DocumentTypes[doc] == docText ||
                DocumentTypes[doc] == docCSS ||
                DocumentTypes[doc] == docSource);
  DoSaveAs(NULL, NULL, TRUE);
  
  if(filename)
    *filename = TtaStrdup(SaveName);
  
  return TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMail (Document document, View view)
{
  char                 buff[MAX_LENGTH], *appname, *vers;
  ElementType          elType;
  Element              docEl, el, text;
  int                  len, i;
  Language             lang;
  EMail                mail;
  wxArrayString        arr;
  SendByMailDlgWX      dlg(0, NULL); 
  char                *temppath;
  char                *server = TtaGetEnvString ("EMAILS_SMTP_SERVER");
  char                *from   = TtaGetEnvString ("EMAILS_FROM_ADDRESS");
  char                *docPath, *docType, *docChar;
  char                *dstFileName = NULL;  
  int                  port;
  int                  error;
  ThotBool             retry = TRUE;

#ifdef _WINDOWS
  temppath = CreateTempDirectory ("sendmail\\");
#else /* _WINDOWS */
  temppath = CreateTempDirectory ("sendmail/");
#endif /* _WINDOWS */
  TtaGetEnvInt ("EMAILS_SMTP_PORT", &port);
  if (server == NULL || from == NULL ||
      server[0] == EOS || from[0] == EOS || port == 0)
  {
    TtaDisplaySimpleMessage (INFO, AMAYA, AM_EMAILS_NO_SERVER);
    // TODO Show the properties dialog at the "emails" tab.
    return;
  }

  Synchronize(document, view); 
  SaveTempCopy(document, temppath, &dstFileName);
  if (DocumentTypes[document] == docHTML)
  {
    docEl = TtaGetMainRoot (document);
    elType = TtaGetElementType(docEl);
    elType.ElTypeNum = HTML_EL_TITLE;
    el = TtaSearchTypedElement (elType, SearchInTree, docEl);
    text = TtaGetFirstChild (el);
    len = TtaGetTextLength(text);
    if (len > 0)
    {
      len = MAX_LENGTH-1;
      TtaGiveTextContent(text, (unsigned char*)buff, &len, &lang);
      buff[len] = EOS;
    }
    else
      buff[0] = EOS;
    dlg.SetSubject(TtaConvMessageToWX (buff));
  }

  // generate the application name
  vers = (char *) TtaGetAppVersion();
  appname = (char *) TtaGetMemory (strlen (vers) + 10);
  sprintf (appname, "Amaya (%s)", vers);
  while (retry)
  {
    if (dlg.ShowModal() == wxID_OK)
    {
      mail = TtaNewEMail ((const char*)dlg.GetSubject().mb_str(wxConvUTF8),
                          (const char*) dlg.GetMessage().mb_str(wxConvUTF8),
                          from);
      if (mail)
      {
        TtaSetMailer(mail, appname);
        
        arr = dlg.GetToRecipients();
        for (i = 0; i < (int)arr.GetCount(); i++)
        {
          wxString rcpt = arr[i];
          rcpt.Trim(true).Trim(false);
          if (!rcpt.IsEmpty ())
            TtaAddEMailToRecipient (mail, (const char*) rcpt.mb_str(wxConvUTF8));
        }
        
        arr = dlg.GetCcRecipients();
        for (i = 0; i < (int)arr.GetCount(); i++)
        {
          wxString rcpt = arr[i];
          rcpt.Trim(true).Trim(false);
          if (!rcpt.IsEmpty ())
            TtaAddEMailCopyRecipient (mail, (const char*) rcpt.mb_str(wxConvUTF8));
        }
        
        
        docPath = GetLocalPath (document, DocumentURLs[document]);
        docType = DocumentMeta[document]->content_type;
        docChar = DocumentMeta[document]->charset;

        // Send document as attachment
        if (dlg.SendAsAttachment ())
          TtaAddEMailAttachmentFile (mail, docType, docPath);
        // Send document as mail message
        else if (dlg.SendAsContent ())
          TtaAddEMailAlternativeFile(mail, docType, docPath, docChar);
        
        // Send all attached files (images, css ...) as attachments.
        if (dlg.SendAsAttachment () || dlg.SendAsContent ())
          {
            wxFileName    msgName (wxString(docPath, wxConvUTF8));
            wxArrayString files;
            wxDir::GetAllFiles (TtaConvMessageToWX(temppath), &files, wxT(""), wxDIR_FILES);
            for (i = 0; i < (int)files.GetCount(); i++)
            {
              wxFileName filename(files[i]);
              if (filename.GetFullName() != wxString(dstFileName, wxConvUTF8))
                TtaAddEMailAttachmentFile(mail, "",
                                          (const char*)filename.GetFullPath().mb_str(wxConvUTF8));
            }
          }
        else if (dlg.SendAsZip()) // Send all by zip file
          {
            wxString zip = wxFileName::CreateTempFileName(wxT("amayazip"));
            if(!zip.IsEmpty() && TtaCreateZipArchive(temppath , (const char*)zip.mb_str(wxConvUTF8)))
              {
                wxFileName zipname(TtaConvMessageToWX(docPath) + wxT(".zip"));
                
                TtaAddEMailAttachmentFileAlternativeName(mail, "application/zip",
                                     (const char*)zip.mb_str(wxConvUTF8),
                                     (const char*)zipname.GetFullName().mb_str(wxConvUTF8));
              }
          }


        error = 0;
        if (TtaSendEMail (mail, server, port, &error))
          TtaSetStatus (document, view, TtaGetMessage (AMAYA, AM_EMAILS_SENT), NULL);
      }

      switch(error)
      {
        case EMAIL_OK:
          retry = FALSE;
          break;
        case EMAIL_SERVER_NOT_RESPOND:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_SERVER_RESPOND);
          break;
        case EMAIL_SERVER_REJECT:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_SERVER_REJECT);
          break;
        case EMAIL_FROM_BAD_ADDRESS:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_FROM_ADDR);
          break;
        case EMAIL_TO_BAD_ADDRESS:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_RCPT_ADDR);
          break;
        case EMAIL_BAD_CONTENT:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_BAD_CONTENT);
          break;
        default:
          TtaDisplaySimpleMessage(INFO, AMAYA, AM_EMAILS_ERR_UNKNOW);
          break;
      }
    }
    else
      break;
  }
  TtaFreeMemory (appname);
  // Remove temp dir content.
  wxArrayString files;
  wxDir::GetAllFiles(wxString(temppath, wxConvUTF8), &files, wxT(""), wxDIR_FILES);
  for (i = 0; i < (int)files.GetCount(); i++)
    wxRemoveFile(files[i]);
  
  wxRmdir(wxString(temppath, wxConvUTF8));
}
