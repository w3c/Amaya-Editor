/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
 *
 * Authors: V. Quint, I. Vatton
 *
 */

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#ifdef TEMPLATES
#include "Template.h"
#include "templates_f.h"
#endif /* TEMPLATES */

#include "XML.h"

#include "anim_f.h"
#include "css_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "fetchXMLname_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "libmanag_f.h"
#include "Mathedit_f.h"
#include "selection.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"
#include "Xml2thot_f.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
#include "bookmarks.h"
#include "BMevent_f.h"
#endif /* BOOKMARKS */
#ifdef DAV
#include "davlib_f.h"
#endif /* DAV */

#ifdef _WX
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */

/* Some prototypes */
static ThotBool     FollowTheLink (Element anchor, Element elSource,
                                   Attribute HrefAttr, Document doc);

/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _FollowTheLink_context {
  Document             doc;
  Element              anchor;
  Element              elSource;
  char                *sourceDocUrl;
  char                *utf8path;
} FollowTheLink_context;


/* info about the last element highlighted when synchronizing with the
   source view */
static Document	    HighlightDocument = 0;
static Element	    HighlightElement = NULL;
static Attribute    HighLightAttribute = NULL;
static ThotBool     Follow_exclusive = FALSE;
static ThotBool     Refresh_exclusive = FALSE;
static ThotBool     SelectionChanging = FALSE;

/*----------------------------------------------------------------------
  GetElemWithAttr
  Search in document doc an element having an attribute of type attrType
  whose value is nameVal.
  Return that element or NULL if not found.
  If ignoreAtt is not NULL, it is an attribute that should be ignored when
  comparing attributes.
  If ignoreEl is not NULL, it is an element that should be ignored when
  comparing attributes.
  ----------------------------------------------------------------------*/
Element GetElemWithAttr (Document doc, AttributeType attrType, char *nameVal,
                         Attribute ignoreAtt, Element ignoreEl)
{
  Element             el, elFound;
  Attribute           nameAttr;
  char               *name;
  int                 length;
  ThotBool            found;

  if (!nameVal)
    return NULL;
  elFound = NULL;
  el = TtaGetMainRoot (doc);
  found = FALSE;
  do
    {
      TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
      if (nameAttr && elFound)
        {
          if (nameAttr != ignoreAtt &&
              /* skip the element which receive the new attribute and
                 element copies */
              elFound != ignoreEl && TtaIsCopy (elFound) == 0)
            {
              length = TtaGetTextAttributeLength (nameAttr);
              length++;
              name = (char *)TtaGetMemory (length);
              if (name != NULL)
                {
                  TtaGiveTextAttributeValue (nameAttr, name, &length);
                  /* compare the NAME attribute */
                  found = (strcmp (name, nameVal) == 0);
                  TtaFreeMemory (name);
                }
            }
        }
      if (!found)
        el = elFound;
    }
  while (!found && elFound);
  if (!found)
    elFound = NULL;
  return elFound;
}

/*----------------------------------------------------------------------
  SearchNAMEattribute
  search in document doc an element having an attribut NAME or ID (defined
  in DTD HTML, MathML, SVG, Template or generic XML) whose value is nameVal.         
  Return that element or NULL if not found.               
  If ignoreAtt is not NULL, it is an attribute that should be ignored when
  comparing NAME attributes.              
  If ignoreEl is not NULL, it is an element that should be ignored when
  comparing NAME attributes.              
  ----------------------------------------------------------------------*/
Element SearchNAMEattribute (Document doc, char *nameVal, Attribute ignoreAtt,
                             Element ignoreEl)
{
  Element             elFound;
  AttributeType       attrType;
  char               *name;

  /* search all elements having an attribute NAME */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);

  if (!elFound)
    {
      /* search all elements having an attribute ID */
      attrType.AttrTypeNum = HTML_ATTR_ID;
      elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
      if (!elFound)
        {
          /* search all elements having an attribute ID */
          attrType.AttrTypeNum = HTML_ATTR_xmlid;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }
    }
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         MathML DTD) */
      attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
      if (attrType.AttrSSchema)
        /* this document uses the MathML DTD */
        {
          attrType.AttrTypeNum = MathML_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
          if (!elFound)
            {
              /* search all elements having an attribute ID */
              attrType.AttrTypeNum = MathML_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
            }
        }
    }
#ifdef _SVG
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         SVG DTD) */
      attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
      if (attrType.AttrSSchema)
        /* this document uses the SVG DTD */
        {
          attrType.AttrTypeNum = SVG_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
          if (!elFound)
            {
              /* search all elements having an attribute ID */
              attrType.AttrTypeNum = SVG_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
            }
        }
    }
#endif /* _SVG */
#ifdef ANNOTATIONS
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         XLink S schema) */
      attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
      if (attrType.AttrSSchema)
        /* this document uses the XLink DTD */
        {
          attrType.AttrTypeNum = XLink_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }
    }
#endif /* ANNOTATIONS */
#ifdef TEMPLATES
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the Template schema) */
      attrType.AttrSSchema = TtaGetSSchema ("Template", doc);
      if (attrType.AttrSSchema)
        {
          attrType.AttrTypeNum = Template_ATTR_xmlid;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }
    }
#endif /* TEMPLATES */          
#ifdef XML_GENERIC
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         XML DTD) */
      attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
      if (attrType.AttrSSchema)
        {
          name = TtaGetSSchemaName (attrType.AttrSSchema);
          if (strcmp(name, "HTML") &&
              strcmp(name, "MathML") &&
              strcmp(name, "SVG"))
            {
              attrType.AttrTypeNum = XML_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal,
                                         ignoreAtt, ignoreEl);
            }
        }
    }
#endif /* XML_GENERIC */

  return (elFound);
}


/*----------------------------------------------------------------------
  CheckUniqueName
  If attribute value is duplicated, generates a parsing error message.
  If it's not a valid ID value, generates a parsing error message.
  ----------------------------------------------------------------------*/
void CheckUniqueName (Element el, Document doc, Attribute attr,
                      AttributeType attrType)
{
#define MaxMsgLength 200
  ElementType    elType;
  int            lineNum, length;
  char          *name;
  char           msgBuffer[MaxMsgLength];

  elType = TtaGetElementType (el);
  if (attr)
    {
      name = GetXMLAttributeName (attrType, elType, doc);
      if (!strcmp (name, "id"))
        {
          if (!TtaIsValidID (attr, FALSE))
            {
              length = TtaGetTextAttributeLength (attr) + 1;
              name = (char *)TtaGetMemory (length);
              TtaGiveTextAttributeValue (attr, name, &length);
              sprintf (msgBuffer, "Invalid ID value %s", name);
              lineNum = TtaGetElementLineNumber(el);
              if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
                XmlParseError (errorParsing, (unsigned char *)msgBuffer, lineNum);
              else
                HTMLParseError (doc, msgBuffer, lineNum);
              TtaFreeMemory (name);
            }
        }
#ifdef IV
      // this function should be optional because it increases the loading time
      else if (MakeUniqueName (el, doc, FALSE, FALSE))
        {
          sprintf (msgBuffer, "Duplicate attribute value %s", name);
          lineNum = TtaGetElementLineNumber(el);
          if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
            XmlParseError (errorParsing, (unsigned char *)msgBuffer, lineNum);
          else
            HTMLParseError (doc, msgBuffer, lineNum);
        }
#endif
    }
}


/*----------------------------------------------------------------------
  FollowTheLink_callback
  This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void FollowTheLink_callback (int targetDocument, int status, char *urlName,
                             char *outputfile, AHTHeaders *http_headers,
                             void *context)
{
  Element             elFound=NULL;
  ElementType         elType;
  Element             elSource;
  Document            doc;
  Element             anchor;
  AttributeType       attrType;
  Attribute           PseudoAttr, HrefAttr;
  SSchema             docSchema; 
  View                view;
  FollowTheLink_context  *ctx = (FollowTheLink_context *) context;
  char               *sourceDocUrl, *utf8path;

  /* retrieve the context */
  if (ctx == NULL)
    return;

  doc = ctx->doc;
  sourceDocUrl = ctx->sourceDocUrl;  
  anchor = ctx->anchor;
  utf8path = ctx->utf8path;
  elSource = ctx->elSource;
  if (utf8path[0] == '#' && targetDocument != 0)
    /* attribute HREF contains the NAME of a target anchor */
    elFound = SearchNAMEattribute (targetDocument, &utf8path[1], NULL, NULL);
  if (DocumentURLs[doc] && !strcmp (DocumentURLs[doc], sourceDocUrl))
    {
      elType = TtaGetElementType (anchor);
      if (elType.ElTypeNum == HTML_EL_Anchor &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          /* it's an HTML A element. Change it's color */
          docSchema =   TtaGetSSchema ("HTML", doc);
          if (docSchema && (doc != targetDocument || utf8path[0] == '#') && anchor)
            {
              /* search PseudoAttr attribute */
              attrType.AttrSSchema = docSchema;
              attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
              PseudoAttr = TtaGetAttribute (anchor, attrType);
              /* if the target document has replaced the clicked
                 document, pseudo attribute "visited" should not be set */
              if (targetDocument == doc)
                /* the target document is in the same window as the
                   source document */
                if (strcmp (sourceDocUrl, DocumentURLs[targetDocument]))
                  /* both document have different URLs */
                  PseudoAttr = NULL;
              /* only turn off the link if it points that exists or that we can
                 follow */
              if (PseudoAttr && status != -1)
                {
                  if (utf8path[0] == '#')
                    {
                      if (targetDocument != 0 && elFound)
                        TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
                    }
                  else
                    TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
                }
            }
        }
    }

  if (utf8path[0] == '#' && targetDocument != 0)
    {
      if (elFound)
        {
          elType = TtaGetElementType (elFound);
          if (elType.ElTypeNum == HTML_EL_LINK &&
              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
            {
              /* the target is a HTML link element, follow this link */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = HTML_ATTR_HREF_;
              HrefAttr = TtaGetAttribute (elFound, attrType);
              if (HrefAttr)
                FollowTheLink (elFound, elSource, HrefAttr, doc);
              return;
            }
          else
            {
              if (targetDocument == doc)
                {
                  /* jump in the same document */
                  /* record current position in the history */
                  AddDocHistory (doc, DocumentURLs[doc], 
                                 DocumentMeta[doc]->initial_url, 
                                 DocumentMeta[doc]->form_data,
                                 DocumentMeta[doc]->method);
                }
              /* show the target element in all views */
              for (view = 1; view < 6; view++)
                if (TtaIsViewOpen (targetDocument, view))
                  TtaShowElement (targetDocument, view, elFound, 0);
            }
        }
    }
  TtaFreeMemory (utf8path);
  TtaFreeMemory (sourceDocUrl);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
  IsCSSLink returns TRUE is the element links a CSS stylesheet.
  ----------------------------------------------------------------------*/
ThotBool IsCSSLink (Element el, Document doc)
{
  AttributeType       attrType;
  Attribute           attr;
  char                buffer[MAX_LENGTH];
  int                 length;

  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_REL;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      /* get a buffer for the attribute value */
      length = MAX_LENGTH;
      TtaGiveTextAttributeValue (attr, buffer, &length);
      if (strncasecmp (buffer, "stylesheet", 10) == 0 ||
          strcasecmp (buffer, "style") == 0)
        {
          /* now check the type of the stylesheet */
          attrType.AttrTypeNum = HTML_ATTR_Link_type;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            /* by default it's a CSS stylesheet */
            return TRUE;
          else
            {
              /* get a buffer for the attribute value */
              length = MAX_LENGTH;
              TtaGiveTextAttributeValue (attr, buffer, &length);
              if (!strcasecmp (buffer, "text/css"))
                return TRUE;
            }
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  FollowTheLink follows the link starting from the anchor element for a
  double click on the elSource element.
  The parameter doc is the document that contains the origin element.
  ----------------------------------------------------------------------*/
static ThotBool FollowTheLink (Element anchor, Element elSource,
                               Attribute HrefAttr, Document doc)
{
  AttributeType          attrType;
  Attribute              PseudoAttr, attr;
  Element                root;
  ElementType            elType;
  Document               targetDocument, reldoc;
  SSchema                HTMLSSchema;
  char                  *pathname, *utf8value;
  char                   documentname[MAX_LENGTH];
  char                  *utf8path, *info, *s;
  int                    length;
  int                    method;
  FollowTheLink_context *ctx;
  ThotBool		            isHTML, history, readonly = FALSE;

  if (Follow_exclusive)
    return FALSE;
  else
    Follow_exclusive = TRUE;
  if (anchor == NULL || HrefAttr == NULL)
    return FALSE;
   
  info = pathname = NULL;
  elType = TtaGetElementType (anchor);
  attrType.AttrTypeNum = 0;
  HTMLSSchema = TtaGetSSchema ("HTML", doc);
  isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
  targetDocument = 0;
  PseudoAttr = NULL;
  /* get a buffer for the target URL */
  length = TtaGetTextAttributeLength (HrefAttr) + 1;
  utf8path = (char *)TtaGetMemory (length);
  if (utf8path)
    {
      elType = TtaGetElementType (anchor);
      if (isHTML && elType.ElTypeNum == HTML_EL_Anchor)
        {
          /* it's an HTML anchor */
          /* attach an attribute PseudoClass = active */
          attrType.AttrSSchema = HTMLSSchema;
          attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
          PseudoAttr = TtaGetAttribute (anchor, attrType);
          if (PseudoAttr == NULL)
            {
              PseudoAttr = TtaNewAttribute (attrType);
              TtaAttachAttribute (anchor, PseudoAttr, doc);
            }
          TtaSetAttributeText (PseudoAttr, "active", anchor, doc);
        }
      /* get the URL itself */
      TtaGiveTextAttributeValue (HrefAttr, utf8path, &length);
      /* suppress white spaces at the end */
      length--;
      while (utf8path[length] == ' ')
        utf8path[length--] = EOS;
       
      /* save the context */
      ctx = (FollowTheLink_context*)TtaGetMemory (sizeof (FollowTheLink_context));
      ctx->anchor = anchor;
      ctx->doc = doc;
      ctx->utf8path = utf8path;
      ctx->elSource = elSource;
      /* save the complete URL of the source document */
      ctx->sourceDocUrl = TtaStrdup (DocumentURLs[doc]);
      TtaSetSelectionMode (TRUE);
      if (utf8path[0] == '#')
        {
          /* the target element is part of the same document */
          targetDocument = doc;
          /* manually invoke the callback */
          FollowTheLink_callback (targetDocument, 0, NULL, NULL, NULL, 
                                  (void *) ctx);
        }
      else
        /* the target element seems to be in another document */
        {
          /* is the source element an image map? */
          if (HTMLSSchema)
            {
              attrType.AttrSSchema = HTMLSSchema;
              attrType.AttrTypeNum = HTML_ATTR_ISMAP;
              attr = TtaGetAttribute (elSource, attrType);
              if (attr)
                {
                  /* it's an image map */
                  utf8value = GetActiveImageInfo (doc, elSource);
                  info = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                      TtaGetDefaultCharset ());
                  TtaFreeMemory (utf8value);
                }
            }
           
          if (info)
            length += strlen (info);
          if (length < MAX_LENGTH)
            length = MAX_LENGTH;
          s = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
                                           TtaGetDefaultCharset ());
          pathname = (char *)TtaGetMemory (length);
          strcpy (pathname, s);
          TtaFreeMemory (s);
          if (info)
            {
              /* @@ what do we do with the precedent parameters?*/
              strcat (pathname, info);
              TtaFreeMemory (info);
            }
          /* interrupt current transfer */
          StopTransfer (doc, 1);	   
          /* get the referred document */
          if (!strncmp (pathname, "mailto:", 7))
            {
              TtaSetStatus (doc, 1,
                            TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                            pathname);
              TtaFreeMemory (pathname);
              TtaFreeMemory (utf8path);
              TtaFreeMemory (ctx);
              Follow_exclusive = FALSE;
              return (FALSE);
            }
#ifdef ANNOTATIONS
          /* is it an annotation link? */
          else if (elType.ElSSchema == TtaGetSSchema ("XLink", doc) &&
                   elType.ElTypeNum == XLink_EL_XLink)
            {
              /* loading an annotation */
              reldoc = 0;
              method = CE_ANNOT;
              history = FALSE;
            }
#endif /* ANNOTATIONS */
          else
            {
              reldoc = doc;
              method = CE_RELATIVE;
              history = TRUE;
              if (isHTML && elType.ElTypeNum == HTML_EL_LINK &&
                  IsCSSLink (anchor, doc))
                {
                  /* opening a CSS */
                  reldoc = 0;
                  method = CE_CSS;
                  history = FALSE;
                  /* normalize the URL */
                  s = (char *)TtaGetMemory (length);
                  strcpy (s, pathname);
                  NormalizeURL (s, doc, pathname, documentname, NULL);
                  TtaFreeMemory (s);
                }
            }
           
          if (method != CE_RELATIVE || DontReplaceOldDoc ||
              CanReplaceCurrentDocument (doc, 1))
            {
              if (IsUndisplayedName (pathname))
                /* it's not necessary to open a new window */
                DontReplaceOldDoc = FALSE;
               
              /* Set the Help document in ReadOnly mode */
              root = TtaGetMainRoot (doc);
              readonly = (TtaGetAccessRight (root) == ReadOnly);
              /* Load the new document */
              targetDocument = GetAmayaDoc (pathname, NULL, reldoc, doc, 
                                            method, history, 
                                            (void (*)(int, int, char*, char*, const AHTHeaders*, void*)) FollowTheLink_callback,
                                            (void *) ctx);
              if (readonly)
                {
                  /* transmit the ReadOnly mode */
                  root = TtaGetMainRoot (targetDocument);
                  TtaSetAccessRight (root, ReadOnly, targetDocument);
                }
            }
          else
            {
              TtaFreeMemory (utf8path);
              TtaFreeMemory (ctx);
            }
          TtaFreeMemory (pathname);
        }
      Follow_exclusive = FALSE;
      return (TRUE);
    }
  Follow_exclusive = FALSE;
  return (FALSE);
}

/*----------------------------------------------------------------------
  CheckRefresh checks if a refresh is requested.
  ----------------------------------------------------------------------*/
void CheckRefresh (Document doc)
{
  Element	 elhead, el;
  ElementType	 elType;
  Attribute      attr;
  AttributeType  attrType, attrType1;
  char           value[MAX_LENGTH], *ptr;
  char          pathname[MAX_LENGTH], documentname[MAX_LENGTH];
  int            length;

  if (Refresh_exclusive)
    return;
  else
    Refresh_exclusive = TRUE;

  if (DocumentTypes[doc] == docHTML)
    /* it's an HTML document */
    {
      el = TtaGetRootElement (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_HEAD;
      elhead = TtaSearchTypedElement (elType, SearchInTree, el);
      el = elhead;
      elType.ElTypeNum = HTML_EL_META;
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_http_equiv;
      attrType1.AttrSSchema = elType.ElSSchema;
      attrType1.AttrTypeNum = HTML_ATTR_meta_content;
      while (el)
        {
          /* look for meta element within the head element */
          el = TtaSearchTypedElementInTree (elType, SearchForward, elhead, el);
          if (el)
            {
              attr = TtaGetAttribute (el, attrType);
              if (attr)
                {
                  value[0] = EOS;
                  length = MAX_LENGTH;
                  TtaGiveTextAttributeValue (attr, value, &length);
                  if (!strcasecmp (value, "refresh"))
                    {
                      attr = TtaGetAttribute (el, attrType1);
                      if (attr)
                        {
                          value[0] = EOS;
                          length = MAX_LENGTH;
                          TtaGiveTextAttributeValue (attr, value, &length);
                          ptr = strstr (value, "URL=");
                          if (ptr == NULL)
                            ptr = strstr (value, "url=");
                          if (ptr)
                            {
                              NormalizeURL (&ptr[4], doc, pathname, documentname, NULL);
                              if (IsUndisplayedName (pathname))
                                /* it's not necessary to open a new window */
                                DontReplaceOldDoc = FALSE;
			      
                              /* Load the new document */
                              doc = GetAmayaDoc (pathname, NULL, doc, doc, 
                                                 CE_RELATIVE, FALSE, 
                                                 NULL, NULL);
                              el = NULL;
                            }
                        }
                    }
                }
            }
        }
    }
  Refresh_exclusive = FALSE;
}

/*----------------------------------------------------------------------
  DblClickOnButton     The user has double-clicked a BUTTON element.         
  ----------------------------------------------------------------------*/
static void DblClickOnButton (Element element, Document document)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  int		       type;

  elType = TtaGetElementType (element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Button_type;
  attr = TtaGetAttribute (element, attrType);
  if (!attr)
    /* default value of attribute type is submit */
    type = HTML_ATTR_Button_type_VAL_submit;
  else
    type = TtaGetAttributeValue (attr);
  if (type == HTML_ATTR_Button_type_VAL_button)
    {
      /**** Activate the corresponding event ****/;
    }
  else
    {
      /* interrupt current transfer */
      StopTransfer (document, 1);	   
      SubmitForm (document, element);
    }
}

/*----------------------------------------------------------------------
  ActivateElement    The user has activated an element.         
  ----------------------------------------------------------------------*/
static ThotBool ActivateElement (Element element, Document document)
{
  AttributeType       attrType;
  Attribute           attr, HrefAttr;
  Element             anchor, elFound;
  ElementType         elType, elType1;
  char               *name;
  ThotBool	       ok, isHTML, isXLink, isSVG;

  elType = TtaGetElementType (element);
  name = TtaGetSSchemaName(elType.ElSSchema);
  isSVG = FALSE;
  isXLink = FALSE;
  isHTML = FALSE;
  if (!strcmp (name, "HTML"))
    isHTML = TRUE;
  else if (!strcmp (name, "XLink"))
    isXLink = TRUE;
  else if (!strcmp (name, "SVG"))
    isSVG = TRUE;

  /* Check if the current element is interested in double clicks */
  ok = FALSE;
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
      elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
    /* it's a basic element. It is interested whatever its namespace */
    ok = TRUE;
  else if (isHTML &&
           (elType.ElTypeNum == HTML_EL_LINK ||
            elType.ElTypeNum == HTML_EL_Anchor ||
            elType.ElTypeNum == HTML_EL_AREA ||
            elType.ElTypeNum == HTML_EL_FRAME ||
            elType.ElTypeNum == HTML_EL_Block_Quote ||
            elType.ElTypeNum == HTML_EL_Quotation ||
            elType.ElTypeNum == HTML_EL_INS ||
            elType.ElTypeNum == HTML_EL_DEL ||
            elType.ElTypeNum == HTML_EL_C_Empty ||
            elType.ElTypeNum == HTML_EL_Radio_Input ||
            elType.ElTypeNum == HTML_EL_Checkbox_Input ||
            elType.ElTypeNum == HTML_EL_Option_Menu ||
            elType.ElTypeNum == HTML_EL_Submit_Input ||
            elType.ElTypeNum == HTML_EL_Reset_Input ||
            elType.ElTypeNum == HTML_EL_BUTTON_ ||
            elType.ElTypeNum == HTML_EL_File_Input))
    ok = TRUE;
  else if (isXLink)
    ok = TRUE;
  else if (isSVG)
    ok = TRUE;

  if (!ok)
    /* DoubleClick is disabled for this element type */
    return (FALSE);

  if (isHTML && (elType.ElTypeNum == HTML_EL_Submit_Input ||
                 elType.ElTypeNum == HTML_EL_Reset_Input))
    /* Form button */
    {
      elType1.ElTypeNum = elType.ElTypeNum;
      if (elType1.ElTypeNum == HTML_EL_Submit_Input ||
          elType1.ElTypeNum == HTML_EL_Reset_Input)
        /* it 's a double click on a submit or reset button */
        {
          /* interrupt current transfer and submit the corresponding form */
          StopTransfer (document, 1);	   
          SubmitForm (document, element);
        }
      else if (elType1.ElTypeNum == HTML_EL_BUTTON_)
        DblClickOnButton (element, document);
      return (TRUE);
    }
  else if (isHTML && (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
                      elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
                      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
                      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT))
    {
      /* is it a double click in a BUTTON element? */
      elType1.ElSSchema = elType.ElSSchema;
      elType1.ElTypeNum = HTML_EL_BUTTON_;
      elFound = TtaGetTypedAncestor (element, elType1);
      if (elFound)
        {
          DblClickOnButton (elFound, document);
          return (TRUE);
        }
      else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = HTML_ATTR_IsInput;
          attr = TtaGetAttribute (element, attrType);
          if (attr)
            /* it's a input image */
            {
              /* interrupt current transfer */
              StopTransfer (document, 1);	   
              SubmitForm (document, element);
              return (TRUE);
            }
        }
    }

  if (isHTML && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      /* is it an option menu ? */
      elFound = TtaGetParent (element);
      elType1 = TtaGetElementType (elFound);
      if (elType1.ElTypeNum == HTML_EL_Option)
        {
          SelectOneOption (document, elFound);
          return (TRUE);
        }
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Option_Menu)
    {
      /* it is an option menu */
      elType1.ElSSchema = elType.ElSSchema;
      elType1.ElTypeNum = HTML_EL_Option;
      elFound = TtaSearchTypedElement (elType1, SearchInTree, element);
      if (elFound)
        {
          SelectOneOption (document, elFound);
          return (TRUE);
        }
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Checkbox_Input)
    {
      SelectCheckbox (document, element);
      return (TRUE);
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Radio_Input)
    {
      SelectOneRadio (document, element);
      return (TRUE);
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_File_Input)
    {
      ActivateFileInput (document, element);
      return (TRUE);
    }

  /* Search the anchor or LINK element */
  anchor = SearchAnchor (document, element, &HrefAttr, FALSE);

  if (anchor && HrefAttr)
    return (FollowTheLink (anchor, element, HrefAttr, document));
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  DisplayUrlAnchor displays the url when an anchor is selected
  ----------------------------------------------------------------------*/
static void DisplayUrlAnchor (Element element, Document document)
{
  Attribute           HrefAttr, titleAttr;
  Element             anchor, ancestor;
  ElementType         elType;
  AttributeType       attrType;
  char                *url, *pathname, *documentname, *utf8value;
  int                 length;

  /* Search an ancestor that acts as a link anchor */
  HrefAttr = NULL;
  anchor = SearchAnchor (document, element, &HrefAttr, FALSE);

  if (anchor && HrefAttr)
    {
      /* Get a buffer for the target URL */
      length = TtaGetTextAttributeLength (HrefAttr);
      length++;
	
      utf8value = (char *)TtaGetMemory (length);
      if (utf8value != NULL)
        {
          /* Get the URL */
          TtaGiveTextAttributeValue (HrefAttr, utf8value, &length);
          url = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                             TtaGetDefaultCharset ());
          TtaFreeMemory (utf8value);
          pathname = (char *)TtaGetMemory (MAX_LENGTH);
          documentname = (char *)TtaGetMemory (MAX_LENGTH);
          if (url[0] == '#')
            {
              strcpy (pathname, DocumentURLs[document]);
              strcat (pathname, url);
            }
          else
            /* Normalize the URL */
            NormalizeURL (url, document, pathname, documentname, NULL);

          /* Display the URL in the status line */
          /* look for a Title attribute */
          titleAttr = NULL;
          ancestor = element;
          do
            {
              elType = TtaGetElementType (ancestor);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                {
                  if (elType.ElTypeNum == HTML_EL_LINK ||
                      elType.ElTypeNum == HTML_EL_Anchor)
                    {
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = HTML_ATTR_Title;
                      titleAttr = TtaGetAttribute (ancestor, attrType);
                    }
                }
              ancestor = TtaGetParent (ancestor);
            }
          while (titleAttr == NULL && ancestor != NULL);
          if (titleAttr)
            {
              TtaFreeMemory (url);
              length = TtaGetTextAttributeLength (titleAttr);
              length ++;
              utf8value = (char *)TtaGetMemory (length);
              if (utf8value)
                {
                  TtaGiveTextAttributeValue (titleAttr, utf8value, &length);
                  url = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                     TtaGetDefaultCharset ());
                  TtaFreeMemory (utf8value);
                  strcat (pathname, " (");
                  strcat (pathname, url);
                  strcat (pathname, ")");
                }
            }
       
          TtaSetStatus (document, 1, pathname, NULL);
          TtaFreeMemory (pathname);
          TtaFreeMemory (documentname);
          TtaFreeMemory (url);
        }
    }
}

/*----------------------------------------------------------------------
  DoAction activates the current element from the keyborad
  ----------------------------------------------------------------------*/
void DoAction (Document doc, View view)
{
  Element             firstSel;
  int                 firstChar, lastChar;

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (firstSel)
    {
      if (!ActivateElement (firstSel, doc))
        TtaSelectWord (firstSel, firstChar, doc, view);
    }
}

/*----------------------------------------------------------------------
  AcceptTab inserts a TAB.
  -----------------------------------------------------------------------*/
ThotBool AcceptTab (NotifyOnTarget *event)
{
  TtcInsertChar (event->document, 1, TAB);
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  NextLinkOrFormElement selects the next link or form element.
  -----------------------------------------------------------------------*/
void NextLinkOrFormElement (Document doc, View view)
{
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             schema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;
#ifdef TEMPLATES
  ElementType         elType1, elType2;
  Element             el2;

  if (DocumentTypes[doc] == docText ||
      DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docSource)
    // insert a tab
    TtcInsertChar (doc, view, 0x9);
  else if (!DocumentMeta[doc]->template_version)
    {
#endif /* TEMPLATES */    
      schema = TtaGetSSchema ("HTML", doc);
      attrType1.AttrTypeNum = HTML_ATTR_NAME;
      attrType1.AttrSSchema = schema;
      attrType2.AttrTypeNum = HTML_ATTR_HREF_;
      attrType2.AttrSSchema = schema;
      root = TtaGetRootElement (doc);
      TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
      if (el == NULL)
        {
          /* start from the root element */
          el = root;
          /* we don't accept to restart from the beginning */
          cycle = TRUE;
        }
      else
        cycle = FALSE;

      /* don't manage this element */
      startEl = el;
      /* we're looking for a next element */
      TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
      found = FALSE;
      while (!found)
        {
          if (el == NULL)
            {
              /* end of the document */
              el = NULL;
              attr = NULL;
              if (!cycle)
                {
                  /* restart from the beginning of the document */
                  cycle = TRUE;
                  el = root;
                }
              else
                /* stop the search */
                found = TRUE;
            }  
          else if (el == startEl)
            {
              /* we made a complete cycle and no other element was found */
              el = NULL;
              attr = NULL;
              found = TRUE;
            }
          else if (attr)
            {
              elType = TtaGetElementType (el);
              switch (elType.ElTypeNum)
                {
                case HTML_EL_Option_Menu:
                case HTML_EL_Checkbox_Input:
                case HTML_EL_Radio_Input:
                case HTML_EL_Submit_Input:
                case HTML_EL_Reset_Input:
                case HTML_EL_Button_Input:
                case HTML_EL_BUTTON_:
                case HTML_EL_Anchor:
                  /* no included text: select the element itself */
                  TtaSelectElement (doc, el);
                  found =TRUE;
                  break;
	      
                case HTML_EL_Text_Area:
                case HTML_EL_Text_Input:
                case HTML_EL_File_Input:
                case HTML_EL_Password_Input:
                  /* look for the last included text */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  child = TtaSearchTypedElement (elType, SearchForward, el);
                  if (child)
                    {
                      next = child;
                      do
                        {
                          child = next;
                          next = TtaSearchTypedElementInTree (elType,
                                                              SearchForward,
                                                              el, child);
                        }
                      while (next);
                      i = TtaGetTextLength (child);
                      TtaSelectString (doc, child, i+1, i);
                    }
                  found =TRUE;
                  break;
	      
                default:
                  break;
                }
            }
          if (!found)
            TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
        }
  
#ifdef TEMPLATES
    }
  else
    {
      /* The document is a template's instance
       * The tab key is used to go to the next
       * editable field */
      schema = TtaGetSSchema ("Template", doc);
      elType1.ElSSchema = schema;
      elType1.ElTypeNum = Template_EL_FREE_STRUCT;
      elType2.ElSSchema = schema;
      elType2.ElTypeNum = Template_EL_FREE_CONTENT;
      
      root = TtaGetRootElement (doc);
      TtaGiveFirstSelectedElement (doc, &startEl, &firstChar, &lastChar);
      /* don't manage this element */
      if (startEl == NULL)
        {
          /* start from the root element */
          startEl = root;
          /* we don't accept to restart from the beginning */
          cycle = TRUE;
        }
      else
        cycle = FALSE;
      
      el = TtaSearchTypedElement (elType1, SearchForward, startEl);
      el2 = TtaSearchTypedElement (elType2, SearchForward, startEl);

      if (el == NULL)
        {
          el = el2;
          if (el == NULL && !cycle)
            {
              /* There was no other element - Search from the beginning*/
              el = TtaSearchTypedElement (elType1, SearchForward, root);
              el2 = TtaSearchTypedElement (elType2, SearchForward, root);
              if (el == NULL)
                el = el2;
            }
        }
      if (el != NULL && el2 != NULL)
        {
          if(TtaIsBefore (el2, el))
            {
              el = el2;
            }
        }
      if (el != NULL)
        {
          /* el should contain the first free_struct or free_content element */
          el = TtaGetLastLeaf (el);
          if (el != NULL)
            {
              firstChar = TtaGetTextLength(el) + 1;
              TtaSelectString (doc, el, firstChar, 0 /* To just put the selector at position firstChar */);

            }
        }
    }
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  PreviousLinkOrFormElement selects the previous link or form element.
  -----------------------------------------------------------------------*/
void PreviousLinkOrFormElement (Document doc, View view)
{
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             schema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;
#ifdef TEMPLATES
  ElementType         elType1, elType2;
  Element             el2;
#endif /* TEMPLATES */    

  if (DocumentTypes[doc] == docText ||
      DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docSource)
    // do nothing
    return;
  schema = TtaGetSSchema ("HTML", doc);
  attrType1.AttrTypeNum = HTML_ATTR_NAME;
  attrType1.AttrSSchema = schema;
  attrType2.AttrTypeNum = HTML_ATTR_HREF_;
  attrType2.AttrSSchema = schema;

#ifdef TEMPLATES
  if (!DocumentMeta[doc]->template_version)
    {
#endif /* TEMPLATES */    
      /* keep in mind the last element of the document */
      root = TtaGetRootElement (doc);
      el = TtaGetLastChild (root);
      attr = NULL;

      while (el)
        {
          root = el;
          /* check if this element matches */
          attr = TtaGetAttribute (el, attrType1);
          if (attr == NULL)
            attr = TtaGetAttribute (el, attrType2);
          if (attr == NULL)
            el = TtaGetLastChild (root);
          else
            /* a right element is found */
            el = NULL;
        }
      TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
      if (el == NULL)
        {
          /* start from the end of the document */
          el = root;
          /* we don't accept to restart from the beginning */
          cycle = TRUE;
          /* attr != 0 if this element matches */
          startEl = NULL;
        }
      else
        {
          cycle = FALSE;
          attr = NULL;
          /* don't manage this element */
          startEl = el;
        }

      if (attr == NULL)
        /* we're looking for a next element */
        TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
      found = FALSE;
      while (!found)
        {
          if (el == NULL)
            {
              /* begginning of the document */
              el = NULL;
              attr = NULL;
              if (!cycle)
                {
                  /* restart from the end of the document */
                  cycle = TRUE;
                  el = root;
                  /* check if this element matches */
                  attr = TtaGetAttribute (el, attrType1);
                  if (attr == NULL)
                    attr = TtaGetAttribute (el, attrType2);
                }
              else
                /* stop the search */
                found = TRUE;
            }  
          else if (el == startEl)
            {
              /* we made a complete cycle and no other element was found */
              el = NULL;
              attr = NULL;
              found = TRUE;
            }
          else if (attr)
            {
              elType = TtaGetElementType (el);
              switch (elType.ElTypeNum)
                {
                case HTML_EL_Option_Menu:
                case HTML_EL_Checkbox_Input:
                case HTML_EL_Radio_Input:
                case HTML_EL_Submit_Input:
                case HTML_EL_Reset_Input:
                case HTML_EL_Button_Input:
                case HTML_EL_BUTTON_:
                case HTML_EL_Anchor:
                  /* no included text: select the element itself */
                  TtaSelectElement (doc, el);
                  found =TRUE;
                  break;
	      
                case HTML_EL_Text_Area:
                case HTML_EL_Text_Input:
                case HTML_EL_File_Input:
                case HTML_EL_Password_Input:
                  /* look for the last included text */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  child = TtaSearchTypedElement (elType, SearchForward, el);
                  if (child)
                    {
                      next = child;
                      do
                        {
                          child = next;
                          next = TtaSearchTypedElementInTree (elType,
                                                              SearchForward,
                                                              el, child);
                        }
                      while (next);
                      i = TtaGetTextLength (child);
                      TtaSelectString (doc, child, i+1, i);
                    }
                  found =TRUE;
                  break;
	      
                default:
                  attr = NULL;
                  break;
                }
            }
          if (!found && !attr)
            TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
        }
#ifdef TEMPLATES
    }
  else
    {
      /* The document is a template's instance
       * The tab key is used to go to the next
       * editable field */
      found = false;
      schema = TtaGetSSchema ("Template", doc);
      elType1.ElSSchema = schema;
      elType1.ElTypeNum = Template_EL_FREE_STRUCT;
      elType2.ElSSchema = schema;
      elType2.ElTypeNum = Template_EL_FREE_CONTENT;
      
      root = TtaGetRootElement (doc);
      TtaGiveFirstSelectedElement (doc, &startEl, &firstChar, &lastChar);
      el = TtaGetParent(startEl);
      
      while (!found && (el != NULL))
        {
          elType = TtaGetElementType(el);
          if ((elType1.ElSSchema == elType.ElSSchema 
               && elType1.ElTypeNum == elType.ElTypeNum) 
              || (elType2.ElSSchema == elType.ElSSchema
                  && elType.ElTypeNum == elType2.ElTypeNum))
            {
              found = true;
            }
          else
            {
              el = TtaGetParent(el);
            }
        }

      if (el!=NULL)
        startEl = el;
      
      /* don't manage this element */
      if (startEl == NULL)
        {
          /* start from the root element */
          startEl = root;
          /* we don't accept to restart from the beginning */
          cycle = TRUE;
        }
      else
        cycle = FALSE;
      
      el = TtaSearchTypedElement (elType1, SearchBackward, startEl);
      el2 = TtaSearchTypedElement (elType2, SearchBackward, startEl);
      
      if (el == NULL)
        {
          el = el2;
          if (el == NULL && !cycle)
            {
              /* There was no other element - Search from the beginning*/
              el = TtaSearchTypedElementInTree (elType1, SearchBackward, root, TtaGetLastLeaf(root));
              el2 = TtaSearchTypedElementInTree (elType2, SearchBackward, root, TtaGetLastLeaf(root));
              if (el == NULL)
                el = el2;
            }

        }

      if (el != NULL && el2 != NULL)
        {
          if(TtaIsBefore (el, el2))
            {
              el = el2;
            }
        }
      if (el != NULL)
        {
          /* el must contain the first free_struct or free_content element */
          el = TtaGetLastLeaf (el);
          if (el != NULL)
            {
              firstChar = TtaGetTextLength(el) + 1;
              TtaSelectString (doc, el, firstChar, 0 /* To just put the selector at position firstChar */);
            }
        }
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  AccessKeyHandler handles links or select elements
  ----------------------------------------------------------------------*/
void AccessKeyHandler (Document doc, void *param)
{
  Element             el, child, next;
  ElementType         elType;
  SSchema             HTMLschema;
  int                 i;

  el = (Element)param;
  if (el)
    {
      elType = TtaGetElementType (el);
      HTMLschema = TtaGetSSchema ("HTML", doc);
      if (TtaSameSSchemas (elType.ElSSchema, HTMLschema) &&
          (elType.ElTypeNum == HTML_EL_LEGEND ||
           elType.ElTypeNum == HTML_EL_LABEL))
        TtaNextSibling (&el);
      /* activate or select the element */
      if (!ActivateElement (el, doc))
        {
          /* look for the last included text */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          child = TtaSearchTypedElement (elType, SearchForward, el);
          if (child)
            {
              next = child;
              do
                {
                  child = next;
                  next = TtaSearchTypedElementInTree (elType, SearchForward, el, child);
                }
              while (next);
              i = TtaGetTextLength (child);
              TtaSelectString (doc, child, i+1, i);
            }
          else
            /* no included text: select the element itself */
            TtaSelectElement (doc, el);
        }
    }
}


/*----------------------------------------------------------------------
  IgnoreEvent       An empty function to be able to ignore events.
  ----------------------------------------------------------------------*/
ThotBool IgnoreEvent (NotifyElement *event)
{
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  DoubleClick     The user has double-clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool DoubleClick (NotifyElement *event)
{
  ThotBool usedouble;


  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    {
      /* don't let Thot perform normal operation */
      return (ActivateElement (event->element, event->document));
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  SimpleClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleClick (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    {
      DisplayUrlAnchor (event->element, event->document);
      return TRUE;
    }
  else
    {
      /* don't let Thot perform normal operation if there is an activation */
      return (ActivateElement (event->element, event->document));
    }
}

/*----------------------------------------------------------------------
  SimpleLClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleLClick (NotifyElement *event)
{
#ifdef _SVG
  ElementType       elType;
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (DocumentTypes[event->document] == docLibrary)
    {
      /* Check the sschema of the document (HTML) */
      elType.ElSSchema = TtaGetDocumentSSchema (event->document);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          /* check the element type */
          elType = TtaGetElementType (event->element);
          if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
            /* Activate Library dialogue because he has selected picture into the document */
            /* Now we are going to browse the document tree to take the url models */
            {
              /* Browse the document tree to save model url */
              SaveSVGURL (event->document, event->element);
              /* Browse the document tree to view line selected */
              ChangeSVGLibraryLinePresentation (event->document, event->element);
              /* Show Drop dialog option */
              CopyOrReference (event->document, 1);
              return TRUE;
            }
        }
    }
#endif /* _SVG */
  /* don't let Thot perform normal operation if there is an activation */
  return FALSE;
}


/*----------------------------------------------------------------------
  SimpleRClick     The user has clicked an element.         
  ----------------------------------------------------------------------*/
ThotBool SimpleRClick (NotifyElement *event)
{
  ThotBool done;
  
#ifdef _WX
  LoadDefaultOpeningLocation (TRUE); // in new frame
  done = ActivateElement (event->element, event->document);
#else /* _WX */
  DontReplaceOldDoc = TRUE;
  done = ActivateElement (event->element, event->document);
  DontReplaceOldDoc = FALSE;
#endif /* _WX*/
  /* don't let Thot perform normal operation if there is an activation */
  return done;
}

/*----------------------------------------------------------------------
  AnnotSimpleClick     The user has clicked on an annotation icon
  ----------------------------------------------------------------------*/
ThotBool AnnotSimpleClick (NotifyElement *event)
{
#ifdef ANNOTATIONS
  /* if it's an annotation link, highlight the annotated text  */
  ANNOT_SelectSourceDoc (event->document, event->element);
#endif /* ANNOTATIONS */
  return SimpleClick (event);
}


/*----------------------------------------------------------------------
  UpdateTitle update the content of the Title field on top of the 
  main window, according to the contents of element el.   
  ----------------------------------------------------------------------*/
void UpdateTitle (Element el, Document doc)
{
  Element             textElem, next, sibling;
  ElementType         elType, siblingType;
  Language            lang;
  char               *text;
#ifndef _WX
  char               *src;
#endif /* _WX */
  int                 length, i, l;

  if (TtaGetViewFrame (doc, 1) == 0)
    /* this document is not displayed */
    return;

  elType = TtaGetElementType (el);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"SVG") &&
      elType.ElTypeNum == SVG_EL_title)
    /* it's a SVG title */
    {
      if (TtaGetParent (el) != TtaGetRootElement(doc))
        /* it's not a child of the root SVG element, ignore */
        return;
      /* this title element is a child of the root element */
      sibling = el;
      do
        {
          TtaPreviousSibling (&sibling);
          if (sibling)
            siblingType = TtaGetElementType (sibling);
        }
      while (sibling && (siblingType.ElTypeNum != SVG_EL_title ||
                         siblingType.ElSSchema != elType.ElSSchema));
      if (sibling)
        /* this title element has a previous title sibling. Ignore */
        return;
    }

  textElem = TtaGetFirstChild (el);
  if (textElem != NULL)
    {
      /* what is the length of the title? */
      length = 0;
      next = textElem;
      while (next)
        {
          elType = TtaGetElementType (next);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            length += TtaGetTextLength (next);
          TtaNextSibling (&next);
        }
      /* get the text of the title */
      length++;
      text = (char *)TtaGetMemory (length);
      text[0] = EOS;
      next = textElem;
      i = 0;
      while (next)
        {
          l = length - i;
          elType = TtaGetElementType (next);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              TtaGiveTextContent (next, (unsigned char *)&text[i], &l, &lang);
              i += l;
            }
          TtaNextSibling (&next);
        }
      if (DocumentTypes[doc] != docSource)
        TtaChangeWindowTitle (doc, 0, text, UTF_8);
      if (DocumentTypes[doc] == docSource || DocumentSource[doc])
        {
#ifdef _WX
          /* with wx, document source use the document filename as title,
           * nothing is done here*/
#else /* _WX */
          src = (char *)TtaGetMemory (length + 9);
          sprintf (src, "Source: %s", text);
          if (DocumentTypes[doc] == docSource)
            TtaChangeWindowTitle (doc, 1, src, UTF_8);
          else
            TtaChangeWindowTitle (DocumentSource[doc], 1, src, UTF_8);
          TtaFreeMemory (src);
#endif /* _WX */
        }
      TtaFreeMemory (text);
    }

}

/*----------------------------------------------------------------------
  CloseLogs closes all logs attached to the document.
  ----------------------------------------------------------------------*/
void CloseLogs (Document doc)
{
  int		     i;

  /* is there log documents linked to this document? */
  if (doc)
    {
      for (i = 1; i < DocumentTableLength; i++)
        if (DocumentURLs[i] && DocumentSource[i] == doc &&
            DocumentTypes[i] == docLog)
          {
            /* close the window of the log file attached to the
               current document */
            TtaCloseDocument (i);
            TtaFreeMemory (DocumentURLs[i]);
            DocumentURLs[i] = NULL;
            /* switch off the button Show Log file */
            TtaSetItemOff (doc, 1, File, BShowLogFile);
            if (DocumentSource[doc])
              TtaSetItemOff (DocumentSource[doc], 1, File, BShowLogFile);
            DocumentSource[i] = 0;
            /* restore the default document type */
            DocumentTypes[i] = docFree;
          }
    }
}

/*----------------------------------------------------------------------
  FreeDocumentResource
  ----------------------------------------------------------------------*/
void FreeDocumentResource (Document doc)
{
  Document	     sourceDoc;
  char              *tempdocument;
  int                i;

  if (doc == 0)
    return;

  TtaSetItemOff (doc, 1, File, BShowLogFile);
  /* unmap the Save as form */
  TtaDestroyDialogue (BaseDialog + SaveForm);
  if (doc == ParsedDoc)
    /* The document to which CSS are to be applied */
    ParsedDoc = 0;
  if (DocumentURLs[doc] != NULL)
    {
      if (DocumentTypes[doc] != docLog)
        {
          /* remove the temporary copy of the file */
          tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
          TtaFileUnlink (tempdocument);
          TtaFreeMemory (tempdocument);
          /* remove the Parsing errors file */
          RemoveParsingErrors (doc);
          ClearMathFrame (doc);
#ifdef ANNOTATIONS
          ANNOT_FreeDocumentResource (doc);
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
          BM_FreeDocumentResource (doc);
#endif /* BOOKMARKS */
        }
      /* remove the document from the auto save list */
      RemoveAutoSavedDoc (doc);
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = NULL;
      if (DocumentMeta[doc])
        {
          DocumentMetaClear (DocumentMeta[doc]);
          TtaFreeMemory (DocumentMeta[doc]);
          DocumentMeta[doc] = NULL;
        }
      if (HighlightDocument == doc)
        ResetHighlightedElement ();

      if (DocumentTypes[doc] == docLog)
        DocumentSource[doc] = 0;
      else
        {
          RemoveDocCSSs (doc);
          /* free access keys table */
          TtaRemoveDocAccessKeys (doc);
          if (!Synchronizing && DocumentSource[doc])
            {
              sourceDoc = DocumentSource[doc];
              // set the source unmodified before closing it
              TtaSetDocumentUnmodified (sourceDoc);
              TtcCloseDocument (sourceDoc, 1);
              FreeDocumentResource (sourceDoc);
              DocumentSource[doc] = 0;
            }
          /* is this document the source of another document? */
          for (i = 1; i < DocumentTableLength; i++)
            if (DocumentURLs[i] && DocumentSource[i] == doc)
              {
                DocumentSource[i] = 0;
                if (DocumentTypes[i] == docLog)
                  {
                    /* close the window of the log file attached to the
                       current document */
                    TtaCloseDocument (i);
                    TtaFreeMemory (DocumentURLs[i]);
                    DocumentURLs[i] = NULL;
                    /* switch off the button Show Log file */
                    TtaSetItemOff (doc, 1, File, BShowLogFile);
                  }
              }
          /* avoid to free images of backup documents */
          if (BackupDocument != doc)
            RemoveDocumentImages (doc);
        }
      /* restore the default document type */
      DocumentTypes[doc] = docFree;
    }
}
 
/*----------------------------------------------------------------------
  DocumentClosed                                                  
  ----------------------------------------------------------------------*/
void DocumentClosed (NotifyDialog * event)
{
  Document            doc;
#ifdef _SVG
  View                tm_view;
#endif  /* _SVG */

  if (event == NULL)
    return;
#ifdef DAV
  /* NEED : deal with last document when exiting the application.
   * 
   * Now, when exiting the application, if the document is locked
   * by the user (the lock information must be in the local base),
   * this function will ask whether the user wants to unlock it.
   * If user agrees, an UNLOCK request will be sent. But, under
   * Windows machines, this request will be killed when the application
   * exit, and no unlock will be done.
   */ 
  DAVFreeLock (event->document);
#endif /* DAV */
  
#ifdef _SVG 
  Get_timeline_of_doc (event->document, &doc, &tm_view);
  if (doc)
    {
      TtaCloseView (doc, tm_view);
      Free_timeline_of_doc (event->document);	   
    }
#endif /* _SVG */
  doc = TtaGetSelectedDocument ();
  if (doc == 0 || doc == event->document)
    {
      /* table elements are no longuer selected */
      TableMenuActive = FALSE;
      MTableMenuActive = FALSE;
      SetTableMenuOff (doc, 1);
    }
  FreeDocumentResource (event->document);
  CleanUpParsingErrors ();  
}

/*----------------------------------------------------------------------
  A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
void UpdateContextSensitiveMenus (Document doc)
{
  ElementType         elType, elTypeSel;
  Element             firstSel;
  SSchema             sch;
  int                 firstChar, lastChar;
  ThotBool            newSelInElem, withHTML, withinTable, inMath;

  if (doc == 0)
    return;
  withHTML = (DocumentTypes[doc] == docHTML && DocumentURLs[doc]);
  withinTable = FALSE;
  inMath = FALSE;
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (firstSel)
    {
      /* check if there is HTML elements */
      sch = TtaGetSSchema ("HTML", doc);
      withHTML = (sch != NULL);

      /* look for an enclosing cell */
      elType = TtaGetElementType (firstSel);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        {
          elType.ElTypeNum = MathML_EL_MTABLE;
          withinTable = (TtaGetTypedAncestor (firstSel, elType) != NULL);
          if (withinTable)
            {
              elType.ElTypeNum = MathML_EL_RowLabel;
              if (TtaGetTypedAncestor (firstSel, elType) != NULL)
                withinTable = FALSE;
            }
          inMath = TRUE;
        }
      elType.ElSSchema = sch;
      if (!withinTable)
        {
          elType.ElTypeNum = HTML_EL_Table_;
          withinTable = (TtaGetTypedAncestor (firstSel, elType) != NULL);
        }
    }

  /* update table menu entries */
  if ((!withinTable || inMath) && TableMenuActive)
    {
      TableMenuActive = FALSE;
      TtaSetItemOff (doc, 1, Types, BCaption);
      TtaSetItemOff (doc, 1, Types, BColgroup);
      TtaSetItemOff (doc, 1, Types, BCol);
      TtaSetItemOff (doc, 1, Types, BTHead);
      TtaSetItemOff (doc, 1, Types, BTBody);
      TtaSetItemOff (doc, 1, Types, BTFoot);
      TtaSetItemOff (doc, 1, Types, BDataCell);
      TtaSetItemOff (doc, 1, Types, BHeadingCell);
      TtaSetItemOff (doc, 1, Types, BCellHExtend);
      TtaSetItemOff (doc, 1, Types, BCellVExtend);
      TtaSetItemOff (doc, 1, Types, BCellHShrink);
      TtaSetItemOff (doc, 1, Types, BCellVShrink);
      TtaSetItemOff (doc, 1, Types, BSelectRow);
      TtaSetItemOff (doc, 1, Types, BCreateRowB);
      TtaSetItemOff (doc, 1, Types, BCreateRowA);
      TtaSetItemOff (doc, 1, Types, BSelectColumn);
      TtaSetItemOff (doc, 1, Types, BCreateColumnB);
      TtaSetItemOff (doc, 1, Types, BCreateColumnA);
      TtaSetItemOff (doc, 1, Types, BPasteBefore);
      TtaSetItemOff (doc, 1, Types, BPasteAfter);
    }
  if ((!withinTable || !inMath) && MTableMenuActive)
    {
      MTableMenuActive = FALSE;
      TtaSetItemOff (doc, 1, XMLTypes, BMCellHExtend);
      TtaSetItemOff (doc, 1, XMLTypes, BMCellVExtend);
      TtaSetItemOff (doc, 1, XMLTypes, BMCellHShrink);
      TtaSetItemOff (doc, 1, XMLTypes, BMCellVShrink);
      TtaSetItemOff (doc, 1, XMLTypes, BMSelectRow);
      TtaSetItemOff (doc, 1, XMLTypes, BMCreateRowB);
      TtaSetItemOff (doc, 1, XMLTypes, BMCreateRowA);
      TtaSetItemOff (doc, 1, XMLTypes, BMSelectColumn);
      TtaSetItemOff (doc, 1, XMLTypes, BMCreateColumnB);
      TtaSetItemOff (doc, 1, XMLTypes, BMCreateColumnA);
      TtaSetItemOff (doc, 1, XMLTypes, BMPasteBefore);
      TtaSetItemOff (doc, 1, XMLTypes, BMPasteAfter);
    }
  if (withinTable && !inMath && !TableMenuActive)
    {
      TableMenuActive = TRUE;
      TtaSetItemOn (doc, 1, Types, BCaption);
      TtaSetItemOn (doc, 1, Types, BColgroup);
      TtaSetItemOn (doc, 1, Types, BCol);
      TtaSetItemOn (doc, 1, Types, BTHead);
      TtaSetItemOn (doc, 1, Types, BTBody);
      TtaSetItemOn (doc, 1, Types, BTFoot);
      TtaSetItemOn (doc, 1, Types, BDataCell);
      TtaSetItemOn (doc, 1, Types, BHeadingCell);
      TtaSetItemOn (doc, 1, Types, BCellHExtend);
      TtaSetItemOn (doc, 1, Types, BCellVExtend);
      TtaSetItemOn (doc, 1, Types, BCellHShrink);
      TtaSetItemOn (doc, 1, Types, BCellVShrink);
      TtaSetItemOn (doc, 1, Types, BSelectRow);
      TtaSetItemOn (doc, 1, Types, BCreateRowB);
      TtaSetItemOn (doc, 1, Types, BCreateRowA);
      TtaSetItemOn (doc, 1, Types, BSelectColumn);
      TtaSetItemOn (doc, 1, Types, BCreateColumnB);
      TtaSetItemOn (doc, 1, Types, BCreateColumnA);
    }
  if (withinTable && inMath && !MTableMenuActive)
    {
      MTableMenuActive = TRUE;
      TtaSetItemOn (doc, 1, XMLTypes, BMCellHExtend);
      TtaSetItemOn (doc, 1, XMLTypes, BMCellVExtend);
      TtaSetItemOn (doc, 1, XMLTypes, BMCellHShrink);
      TtaSetItemOn (doc, 1, XMLTypes, BMCellVShrink);
      TtaSetItemOn (doc, 1, XMLTypes, BMSelectRow);
      TtaSetItemOn (doc, 1, XMLTypes, BMCreateRowB);
      TtaSetItemOn (doc, 1, XMLTypes, BMCreateRowA);
      TtaSetItemOn (doc, 1, XMLTypes, BMSelectColumn);
      TtaSetItemOn (doc, 1, XMLTypes, BMCreateColumnB);
      TtaSetItemOn (doc, 1, XMLTypes, BMCreateColumnA);
    }
  if (withinTable && TtaIsColumnRowSelected (doc))
    {
      if (inMath)
        {
          TtaSetItemOn (doc, 1, XMLTypes, BMPasteBefore);
          TtaSetItemOn (doc, 1, XMLTypes, BMPasteAfter);
        }
      else
        {
          TtaSetItemOn (doc, 1, Types, BPasteBefore);
          TtaSetItemOn (doc, 1, Types, BPasteAfter);
        }
    }

  if (!withHTML)
    return;
  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Preformatted;
      newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }

  /* 
   * elements PICTURE, Object, Applet, Big_text, Small_text, Subscript,
   * Superscript, Font_  are not permitted in a Preformatted element.
   * The corresponding menu entries must be turned off 
   */
  if (newSelInElem != SelectionInPRE)
    {
      SelectionInPRE = newSelInElem;
      if (newSelInElem)
        {
          TtaSetItemOff (doc, 1, Types, BImage);
          TtaSetItemOff (doc, 1, Types, BObject);
          TtaSetItemOff (doc, 1, Types, TBig);
          TtaSetItemOff (doc, 1, Types, TSmall);
          TtaSetItemOff (doc, 1, Types, TSub);
          TtaSetItemOff (doc, 1, Types, TSup);
        }
      else
        {
          TtaSetItemOn (doc, 1, Types, BImage);
          TtaSetItemOn (doc, 1, Types, BObject);
          TtaSetItemOn (doc, 1, Types, TBig);
          TtaSetItemOn (doc, 1, Types, TSmall);
          TtaSetItemOn (doc, 1, Types, TSub);
          TtaSetItemOn (doc, 1, Types, TSup);
        }
    }
  /* 
   * Disable the "Comment" entry of menu "Context" if current selection
   * is within a comment 
   */
  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Comment_;
      newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (newSelInElem != SelectionInComment)
    {
      SelectionInComment = newSelInElem;
      if (newSelInElem)
        TtaSetItemOff (doc, 2, StructTypes, BComment);
      else
        TtaSetItemOn (doc, 2, StructTypes, BComment);
    }
  /* update toggle buttons in menus "Information Type" and */
  /* "Character Element" */
  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Emphasis;
      elTypeSel = TtaGetElementType (firstSel);
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInEM != newSelInElem)
    {
      SelectionInEM = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TEmphasis, newSelInElem);
#ifndef _WX
      TtaSwitchButton (doc, 1, iI);
#else /* _WX */
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_EMPH, newSelInElem );
#endif /* _WX */	
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Strong;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInSTRONG != newSelInElem)
    {
      SelectionInSTRONG = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TStrong, newSelInElem);
#ifndef _WX
      TtaSwitchButton (doc, 1, iB);
#else /* _WX */
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_STRONG, newSelInElem );
#endif /* _WX */
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Cite;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInCITE != newSelInElem)
    {
      SelectionInCITE = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TCite, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_ABBR;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInABBR != newSelInElem)
    {
      SelectionInABBR = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TAbbreviation, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_ACRONYM;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInACRONYM != newSelInElem)
    {
      SelectionInACRONYM = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TAcronym, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_INS;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInINS != newSelInElem)
    {
      SelectionInINS = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TInsertion, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_DEL;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInDEL != newSelInElem)
    {
      SelectionInDEL = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TDeletion, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Def;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInDFN != newSelInElem)
    {
      SelectionInDFN = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TDefinition, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Code;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInCODE != newSelInElem)
    {
      SelectionInCODE = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TCode, newSelInElem);
#ifndef _WX
      TtaSwitchButton (doc, 1, iT);
#else /* _WX */
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_CODE, newSelInElem );
#endif /* _WX */
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Variable_;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInVAR != newSelInElem)
    {
      SelectionInVAR = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TVariable, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Sample;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInSAMP != newSelInElem)
    {
      SelectionInSAMP = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSample, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Keyboard;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInKBD != newSelInElem)
    {
      SelectionInKBD = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TKeyboard, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Italic_text;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInI != newSelInElem)
    {
      SelectionInI = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TItalic, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Bold_text;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInB != newSelInElem)
    {
      SelectionInB = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBold, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Teletype_text;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInTT != newSelInElem)
    {
      SelectionInTT = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TTeletype, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Big_text;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInBIG != newSelInElem)
    {
      SelectionInBIG = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBig, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Small_text;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInSMALL != newSelInElem)
    {
      SelectionInSMALL = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSmall, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Subscript;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInSub != newSelInElem)
    {
      SelectionInSub = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSub, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Superscript;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInSup != newSelInElem)
    {
      SelectionInSup = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSup, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Quotation;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInQuote != newSelInElem)
    {
      SelectionInQuote = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TQuotation, newSelInElem);
    }

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_BDO;
      if (elTypeSel.ElTypeNum == elType.ElTypeNum &&
          elTypeSel.ElSSchema == elType.ElSSchema)
        newSelInElem = TRUE;
      else
        newSelInElem = (TtaGetTypedAncestor (firstSel, elType) != NULL);
    }
  if (SelectionInBDO != newSelInElem)
    {
      SelectionInBDO = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBDO, newSelInElem);
    }
}

/*----------------------------------------------------------------------
  LineNumberOfEl
  Returns the line number (position in the source file) of element el.
  ----------------------------------------------------------------------*/
static int LineNumberOfEl (Element el)
{
  int		ln;
  Element	child, sibling, uncle, ancestor, prev, parent;

  ln = TtaGetElementLineNumber (el);
  if (ln == 0)
    /* there is no line number associated with this element: the element
       does not exist in the source file */
    {
      /* get the first line number associated with its descendants */
      child = TtaGetFirstChild (el);
      while (child && ln == 0)
        {
          ln = LineNumberOfEl (child);
          if (ln == 0)
            TtaNextSibling (&child);
        }
      if (ln == 0)
        /* Descendants don't have any line number. Get the first line number
           associated with its following siblings */
        {
          sibling = el;
          do
            {
              TtaNextSibling (&sibling);
              if (sibling)
                ln = LineNumberOfEl (sibling);
            }
          while (sibling && ln == 0);
        }
      if (ln == 0)
        /* Siblings don't have any line number. Get the first line number
           associated with the following siblings of its ancestors */
        {
          ancestor = el;
          do
            {
              ancestor = TtaGetParent (ancestor);
              if (ancestor)
                {
                  uncle = ancestor;
                  do
                    {
                      TtaNextSibling (&uncle);
                      if (uncle)
                        ln = LineNumberOfEl (uncle);
                    }
                  while (uncle && ln == 0);
                }
            }
          while (ancestor && ln == 0);
        }
      if (ln == 0)
        /* Still no line number. Get the line number of the previous
           element with a line number */
        {
          ancestor = el;
          prev = el;
          TtaPreviousSibling (&prev);
          while (prev == NULL && ln == 0 && ancestor != NULL)
            {
              ancestor = TtaGetParent (ancestor);
              ln = TtaGetElementLineNumber (ancestor);
              if (ln == 0)
                {
                  prev = ancestor;
                  TtaPreviousSibling (&prev);
                }
            }
          ancestor = prev;

          while (ancestor && ln == 0)
            {
              prev = TtaGetLastLeaf (ancestor);
              if (prev)
                {
                  parent = TtaGetParent (prev);
                  while (prev && ln == 0)
                    {
                      ln = TtaGetElementLineNumber (prev);
                      if (ln == 0)
                        TtaPreviousSibling (&prev);
                    }
                  if (ln == 0)
                    ln = TtaGetElementLineNumber (parent);
                  if (ln == 0)
                    ancestor = TtaGetPredecessor (parent);
                }
            }
        }
    }
  return ln;
}

/*----------------------------------------------------------------------
  ResetHighlightedElement
  If an element is currently highlighted, remove its Highlight attribute
  ----------------------------------------------------------------------*/
void ResetHighlightedElement ()
{
  if (HighlightElement)
    {
      if (TtaGetElementType (HighlightElement).ElSSchema != NULL)
        TtaRemoveAttribute (HighlightElement, HighLightAttribute,
                            HighlightDocument);
      HighlightDocument = 0;
      HighlightElement = NULL;
      HighLightAttribute = NULL;
    }
}

/*----------------------------------------------------------------------
  SynchronizeSourceView
  A new element has been selected. If the Source view is open,
  synchronize it with the new selection.      
  ----------------------------------------------------------------------*/
void SynchronizeSourceView (NotifyElement *event)
{
  Element             firstSel, el, child, otherEl, root;
  ElementType         elType;
  AttributeType       attrType;
  Attribute	      attr;
  Document	      doc, otherDoc;
  char                message[50];
  int                 firstChar, lastChar, line, i, view;
  int		      val, x, y, width, height;
  ThotBool	      otherDocIsStruct, done;
   
  if (!event)
    return;
  doc = event->document;
  done = FALSE;
  /* get the other Thot document to be synchronized with the one where the
     user has just clicked */
  otherDoc = 0;
  otherDocIsStruct = FALSE;
  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docLibrary ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docSVG  ||
      DocumentTypes[doc] == docXml)
    /* the user clicked on a structured document, the other doc is the
       corresponding source document */
    otherDoc = DocumentSource[doc];
  else if (DocumentTypes[doc] == docSource)
    /* the user clicked on a source document, the other doc is the
       corresponding structured document */
    {
      otherDocIsStruct = TRUE;
      for (i = 1; i < DocumentTableLength; i++)
        if (DocumentURLs[i] &&
            (DocumentTypes[i] == docHTML ||
             DocumentTypes[i] == docLibrary ||
             DocumentTypes[i] == docMath ||
             DocumentTypes[i] == docSVG ||
             DocumentTypes[i] == docXml) &&
            DocumentSource[i] == doc)
          {
            otherDoc = i;
            i = DocumentTableLength;
          }
    }

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (otherDoc)
    /* looks for the element in the other document that corresponds to
       the clicked element */
    {
      if (firstSel == NULL && event->document == doc)
        /* the current selection is not already registered */
        firstSel = event->element;
      if (firstSel)
        {
          otherEl = NULL;
          /* Get the line number associated with the clicked element */
          line = LineNumberOfEl (firstSel);
          if (line == 0)
            return;
          /* look for an element with the same line number in the other doc */
          /* line numbers are increasing in document order */
          root = TtaGetMainRoot (otherDoc);
          el = root;
          elType = TtaGetElementType (el);
          do
            {
              if (el != root && TtaGetElementLineNumber (el) >= line)
                /* that's the right element */
                otherEl = el;
              else
                {
                  child = TtaGetFirstChild (el);
                  if (!child)
                    otherEl = el;
                  else
                    {
                      do
                        {
                          el = child;
                          TtaNextSibling (&child);
                        }
                      while (child && LineNumberOfEl (child) <= line);
                    }
                }
            }
          while (!otherEl && el);
	 
          done = (otherEl == HighlightElement);
          if (otherEl && !done)
            /* different element found */
            {
              /* If an element is currently highlighted, remove its Highlight
                 attribute */
              ResetHighlightedElement ();
              /* Put a Highlight attribute on the element found */
              if (otherDocIsStruct)
                {
                  if (DocumentTypes[otherDoc] == docHTML)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("HTML",
                                                            otherDoc);
                      attrType.AttrTypeNum = HTML_ATTR_Highlight;
                      val = HTML_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docMath)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("MathML",
                                                            otherDoc);
                      attrType.AttrTypeNum = MathML_ATTR_Highlight;
                      val = MathML_ATTR_Highlight_VAL_Yes_;
                    }
#ifdef _SVG
                  else if (DocumentTypes[otherDoc] == docSVG)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("SVG",
                                                            otherDoc);
                      attrType.AttrTypeNum = SVG_ATTR_Highlight;
                      val = SVG_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docXml)
                    {
                      /* We take the schema of the main root */
                      /* attrType.AttrSSchema = TtaGetSSchema ("XML",
                         otherDoc); */
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = XML_ATTR_Highlight;
                      val = XML_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docLibrary)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("HTML",
                                                            otherDoc);
                      attrType.AttrTypeNum = HTML_ATTR_Highlight;
                      val = HTML_ATTR_Highlight_VAL_Yes_;
                    }
#endif /* _SVG */
                  else
                    {
                      attrType.AttrSSchema = NULL;
                      attrType.AttrTypeNum = 0;
                      val = 0;
                    }
                }
              else
                {
                  attrType.AttrSSchema = TtaGetSSchema ("TextFile",
                                                        otherDoc);
                  attrType.AttrTypeNum = TextFile_ATTR_Highlight;
                  val = TextFile_ATTR_Highlight_VAL_Yes_;
                }
              if (attrType.AttrSSchema)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (otherEl, attr, otherDoc);
                  TtaSetAttributeValue (attr, val, otherEl, otherDoc);
                  /* record the highlighted element */
                  HighlightDocument = otherDoc;
                  HighlightElement = otherEl;
                  HighLightAttribute = attr;
                  /* Scroll all views where the element appears to show it */
                  for (view = 1; view < 6; view++)
                    if (TtaIsViewOpen (otherDoc, view))
                      {
                        TtaGiveBoxAbsPosition (otherEl, otherDoc, view, UnPixel,
                                               &x, &y);
                        TtaGiveWindowSize (otherDoc, view, UnPixel, &width,
                                           &height);
                        if (y < 0 || y > height - 15)
                          TtaShowElement (otherDoc, view, otherEl, 25);
                      }
                }
              done = TRUE;
            }
        }
    }

  if (!done)
    /* If an element is currently highlighted, remove its Highlight
       attribute */
    ResetHighlightedElement ();

  if (firstSel &&
      (DocumentTypes[doc] == docSource ||
       DocumentTypes[doc] == docText ||
       DocumentTypes[doc] == docCSS ||
       DocumentTypes[doc] == docLog))
    {
      /* display the line position of the selection */
      line = TtaGetElementLineNumber (firstSel);
      elType = TtaGetElementType (firstSel);
      if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
        {
          /* take into account previous elements in the same line */
          el = TtaGetParent (firstSel);
          el = TtaGetFirstChild (el);
          while (el && el != firstSel)
            {
              /* add characters of previous elements */
              firstChar += TtaGetElementVolume (el);
              TtaNextSibling (&el);
            }
        }
      sprintf (message, "char %d", firstChar);
      TtaSetStatus (doc, 1, message, NULL);
    }
}

/*----------------------------------------------------------------------
  GetCurrentLine returns the current selected line char index in the
  source file.
  -----------------------------------------------------------------------*/
void GetCurrentLine (Document doc, int *line, int *index)
{
  Element             el, child, parent;
  ElementType         elType;
  int                 first, last;

  *line = 0;
  *index = 0;
  /* look for a selection in the current document */
  TtaGiveFirstSelectedElement (doc, &el, &first, &last);
  if (el)
    {
      *line = TtaGetElementLineNumber (el);
      *index = first;
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == TextFile_EL_Line_)
        {
          /* take into account previous elements in the same line */
          parent = TtaGetParent (el);
          child = TtaGetFirstChild (parent);
          while (child != el)
            {
              /* add characters of previous elements */
              *index += TtaGetElementVolume (child);
              TtaNextSibling (&child);
            }
        }
    }
}

/*----------------------------------------------------------------------
  GotoLine points the corresponding line and char index in the
  source file.
  -----------------------------------------------------------------------*/
void GotoLine (Document doc, int line, int index, ThotBool selpos)
{
  Element             el, child, prev;
  ElementType         elType;
  char                message[50];
  int                 i, len;

  if (line)
    {
      /* open the source file */
      if (DocumentTypes[doc] != docCSS && DocumentTypes[doc] != docSource)
        {
          if (DocumentSource[doc] == 0)
            ShowSource (doc, 1);
          doc = DocumentSource[doc];
        }
      TtaRaiseView (doc, 1);
      /* look for an element with the same line number in the other doc */
      /* line numbers are increasing in document order */
      el = TtaGetMainRoot (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      for (i = 1; i < line; i++)
        TtaNextSibling (&el);
      if (el)
        {
          child = TtaGetFirstChild (el);
          if (child)
            {
              if (index > 0)
                {
                  i = index;
                  len = TtaGetElementVolume (child);
                  while (child && len < i)
                    {
                      /* skip previous elements in the same line */
                      i -= len;
                      prev = child;
                      TtaNextSibling (&child);
                      if (child == NULL)
                        {
                          len = i;
                          child = prev;
                        }
                      else
                        len = TtaGetElementVolume (child);
                    }
                  if (selpos)
                    TtaSelectString (doc, child, i, i-1);
                  else
                    TtaSelectString (doc, child, i, i);
                }
              else
                TtaSelectElement (doc, el);
              sprintf (message, "char %d", index);
              TtaSetStatus (doc, 1, message, NULL);
            }
        }
      else
        TtaSetStatus (doc, 1, "   ", NULL);
    }
}

/*----------------------------------------------------------------------
  ShowTextLine points the corresponding line.
  -----------------------------------------------------------------------*/
static ThotBool ShowTextLine (Element el, Document doc)
{
  Document	      otherDoc = 0;
  Element             otherEl;
  Language            lang;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  char               *utf8value = NULL, *ptr = NULL, *s = NULL;
  int                 len, line = 0, index = 0;

  if (DocumentTypes[doc] == docLog)
    {
      /* get the target line and index from current string */
      if (el)
        {
          len = TtaGetTextLength (el);
          if (len > 0)
            {
              utf8value = (char *)TtaGetMemory (len + 1);
              TtaGiveTextContent (el, (unsigned char *)utf8value, &len, &lang);
              /* extract the line number and the index within the line */
              ptr = strstr (utf8value, "line ");
              if (ptr)
                sscanf (&ptr[4], "%d", &line);
              if (ptr)
                ptr = strstr (ptr, "char");
              if (ptr)
                sscanf (&ptr[4], "%d", &index);
              /* Is there a file name in the current line */
              ptr = strstr (utf8value, ", file ");
              if (ptr)
                ptr += 7;
            }

          /* get the target document */
          otherDoc = DocumentSource[doc];
          if (ptr == NULL)
            {
              otherEl = TtaSearchText (doc, el, FALSE, "***", ISO_8859_1);
              if (otherEl)
                {
                  TtaFreeMemory (utf8value);
                  len = TtaGetTextLength (otherEl) + 1;
                  utf8value = (char *)TtaGetMemory (len);
                  TtaGiveTextContent (otherEl, (unsigned char *)utf8value,
                                      &len, &lang);
                  ptr = strstr (utf8value, " in ");
                  if (ptr)
                    ptr += 4;
                }
            }

          if (ptr)
            s = (char *)TtaConvertMbsToByte ((unsigned char *)ptr,
                                             TtaGetDefaultCharset ());
          if (DocumentURLs[otherDoc] &&
              s && strcmp (s, DocumentURLs[otherDoc]))
            {
              /* it doesn't concern the source document itself
                 look or the target file */
              for (otherDoc = 1; otherDoc < MAX_DOCUMENTS; otherDoc++)
                if (DocumentURLs[otherDoc] &&
                    !strcmp (s, DocumentURLs[otherDoc]))
                  break;
              if (otherDoc == MAX_DOCUMENTS)
                {
                  /* not found: do we have to open a CSS file */
                  css = SearchCSS (0, s, NULL, &pInfo);
                  if (css)
                    {
#ifdef _WX
                      LoadDefaultOpeningLocation (TRUE); // in new frame
                      otherDoc = GetAmayaDoc (s, NULL, DocumentSource[doc], DocumentSource[doc], CE_CSS,
                                              FALSE, NULL, NULL);
#else /* _WX */
                      otherDoc = GetAmayaDoc (s, NULL, 0, 0, CE_CSS,
                                              FALSE, NULL, NULL);
#endif /* _WX */
                    }
                }
            }
        }

      TtaFreeMemory (s);
      TtaFreeMemory (utf8value);
      /* skip to the line */
      if (line && otherDoc && otherDoc < MAX_DOCUMENTS)
        GotoLine (otherDoc, line, index, FALSE);
      return TRUE; /* don't let Thot perform normal operation */
    }
  else
    return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  SimpleClickInText The user has clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool SimpleClickInText (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    return TRUE;
  else
    /* don't let Thot perform normal operation if there is an activation */
    return (ShowTextLine (event->element, event->document));
}

/*----------------------------------------------------------------------
  DoubleClickInText The user has double-clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool DoubleClickInText (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);  
  if (usedouble)
    /* don't let Thot perform normal operation */
    return (ShowTextLine (event->element, event->document));
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  RightClickInText The user has right-clicked a log message.         
  ----------------------------------------------------------------------*/
ThotBool RightClickInText (NotifyElement *event)
{
  return (ShowTextLine (event->element, event->document));
}

/*----------------------------------------------------------------------
  CheckSynchronize
  Check if the selected document must be synchronized.
  If the clicked document is not the current one : synchronize it !
  ----------------------------------------------------------------------*/
void CheckSynchronize (NotifyElement *event)
{
  if (event->document != SelectionDoc &&
      DocumentURLs[event->document] != NULL &&
      DocumentTypes[event->document] != docLog) /* it's not a Log file */
    {
      if (SelectionDoc && DocumentURLs[SelectionDoc])
        {
          /* Reset buttons state in previous selected document */
          UpdateContextSensitiveMenus (SelectionDoc);
          /* Synchronize the content of the old document */
          if (DocumentTypes[SelectionDoc] == docSource || /* source of ... */
              (DocumentSource[SelectionDoc] && /* has a source */
               (DocumentTypes[SelectionDoc] == docHTML ||
                DocumentTypes[SelectionDoc] == docSVG ||
                DocumentTypes[SelectionDoc] == docLibrary ||
                DocumentTypes[SelectionDoc] == docMath ||
                DocumentTypes[SelectionDoc] == docXml)))
            {
              if (event->info == 1)
                /* an undo operation was done in event->document */
                DoSynchronize (event->document, 1, event);
              else if (TtaIsDocumentUpdated (SelectionDoc))
                DoSynchronize (SelectionDoc, 1, event);
              else if (event->document == HighlightDocument)
                SynchronizeSourceView (event);
            }
          // update menus with the new selection
          UpdateEditorMenus (event->document);
        }
      else
        {
          /* the document didn't change. Only synchronize the selection. */
          UpdateContextSensitiveMenus (event->document);
          SynchronizeSourceView (event);
        }
      SelectionDoc = event->document;
    }
  else
    {
      /* the document didn't change. Only synchronize the selection. */
      UpdateContextSensitiveMenus (event->document);
      SynchronizeSourceView (event);
    }
}

/*----------------------------------------------------------------------
  A new element has been selected. Update menus accordingly.      
  ----------------------------------------------------------------------*/
void SelectionChanged (NotifyElement *event)
{
  if (SelectionChanging)
    return;
  SelectionChanging = TRUE;
  CheckSynchronize (event);
  TtaSelectView (SelectionDoc, 1);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
  UnFrameMath ();
  SelectionChanging = FALSE;
}

/*----------------------------------------------------------------------
  HTMLElementTypeInMenu
  -----------------------------------------------------------------------*/
ThotBool HTMLElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
                  element creation menu */
}

/*----------------------------------------------------------------------
  ResetFontOrPhrase: The element elem is removed.
  ----------------------------------------------------------------------*/
static void ResetFontOrPhrase (Document doc, Element elem)
{
  Element      next, child, first, last;

  if (elem)
    {
      child = TtaGetFirstChild (elem);
      first = NULL;
      while (child)
        {
          // next element
          next = child;
          TtaNextSibling (&next);
          TtaRegisterElementDelete (child, doc);
          TtaRemoveTree (child, doc);
          TtaInsertSibling (child, elem, TRUE, doc);
          TtaRegisterElementCreate (child, doc);
          if (first == NULL)
            first = child;
          last = child;
          child = next;
        }
      TtaRegisterElementDelete (elem, doc);
      TtaRemoveTree (elem, doc);
      TtaSetDocumentModified (doc);
      TtaSelectElement (doc, first);
      if (last != first)
        TtaExtendSelection (doc, last, TtaGetElementVolume (last) + 1);
    }
}


/*----------------------------------------------------------------------
  SetCharFontOrPhrase
  ----------------------------------------------------------------------*/
void SetCharFontOrPhrase (int doc, int elemtype)
{
  Element             firstSel, lastSel, el, parent;
  ElementType         elType, parentType;
  DisplayMode         dispMode;
  int                 firstSelectedChar, lastSelectedChar, i;
  ThotBool            remove;

  if (!TtaGetDocumentAccessMode (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstSelectedChar,
                               &lastSelectedChar);
  if (firstSel == NULL)
    {
      /* no selection available */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }

  TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastSelectedChar);
  elType = TtaGetElementType (firstSel);
  parent = NULL;
  remove = FALSE;
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      // check if a typed element is selected
      if (elType.ElTypeNum == elemtype)
        parent = firstSel;
      remove = (firstSel == lastSel);
    }
  else
    elType.ElSSchema = TtaGetSSchema ("HTML", doc);

  if (parent == NULL)
    {
      if ( TtaIsSelectionEmpty ())
        {
          // check if the user wants to close the current element
          parent = TtaGetParent (firstSel);
          parentType = TtaGetElementType (parent);
          i =  TtaGetElementVolume (firstSel);
          if (parentType.ElSSchema == elType.ElSSchema &&
              parentType.ElTypeNum == elemtype &&
              elType.ElTypeNum == HTML_EL_TEXT_UNIT && lastSelectedChar >= i &&
              firstSel == TtaGetLastChild (parent))
            {
              TtcCreateElement (doc, 1);
              return;
            }
        }
      elType.ElTypeNum = elemtype;
      parent = TtaGetTypedAncestor (firstSel, elType);
      // check if the whole selection is included by the same parent
      el = lastSel;
      while (parent && el && el != parent)
        el = TtaGetParent (el);
      if (el == parent)
       remove = TRUE;
    }

  if (parent && !remove)
    {
      /* the selected element is read-only */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_INLINE);
      return;
    }

  /* don't display immediately every change made to the document structure */
  dispMode = TtaGetDisplayMode (doc);
  if (parent)
    {
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      TtaClearViewSelections ();
      TtaOpenUndoSequence (doc, firstSel, lastSel, firstSelectedChar, lastSelectedChar);
      ResetFontOrPhrase (doc, parent);
      TtaCloseUndoSequence (doc);
      /* retore the display mode */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
  else
    {
      TtaOpenUndoSequence (doc, firstSel, lastSel, firstSelectedChar, lastSelectedChar);
      GenerateInlineElement (elemtype, 0, "");
      TtaCloseUndoSequence (doc);      
    }

  UpdateContextSensitiveMenus (doc);
}


/*----------------------------------------------------------------------
  CopyLocation
  Store the current URI (document URL + ID/name) into the clipboard
  ----------------------------------------------------------------------*/
void CopyLocation (Document doc, View view)
{
#ifdef _WX
  TtaStringToClipboard ((unsigned char *)DocumentURLs[doc], UTF_8);
#else /* _WX */
  TtaStringToClipboard ((unsigned char *)DocumentURLs[doc], TtaGetLocaleCharset());
#endif /* _WX */
}

/*----------------------------------------------------------------------
  PasteLocation
  Paste the clipboard value into the document address
  ----------------------------------------------------------------------*/
void PasteLocation (Document doc, View view)
{
}

/*----------------------------------------------------------------------
  CopyLink
  If current selection is within an anchor, store the link value into the
  clipboard.
  ----------------------------------------------------------------------*/
void CopyLink (Document doc, View view)
{
}
