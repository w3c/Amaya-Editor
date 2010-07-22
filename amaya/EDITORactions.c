/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Menu bar functions of Amaya application.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "amaya.h"
#include "css.h"
#include "undo.h"
#include "document.h"
#include "MENUconf.h"
#include "paneltypes_wx.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#include "ANNOTmenu_f.h"
#endif /* ANNOTATIONS */
#ifdef TEMPLATES
#include "Template.h"
#include "templates.h"
#include "templateUtils_f.h"
#include "templates_f.h"
#endif /* TEMPLATES */

#ifdef BOOKMARKS
#include "BMevent_f.h"
#endif /* BOOKMARKS */

#include "css_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "fetchXMLname_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "HTMLsave_f.h"
#include "HTMLtable_f.h"
#include "MathMLbuilder_f.h"
#include "MENUconf_f.h"
#include "styleparser_f.h"
#include "UIcss_f.h"
#include "SVGbuilder_f.h"
#include "SVGedit_f.h"
#include "XHTMLbuilder_f.h"
#include "wxdialogapi_f.h"
#include "appdialogue_wx.h"
#include "SVGedit_f.h"
#include "wxdialog/ListNSDlgWX.h"

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib_f.h"
#endif /* DAV */
#ifdef _JAVA
  #include "javascript_f.h"
#endif /* _JAVA */

static void * NSList = NULL;

/*----------------------------------------------------------------------
  Switch_JS_DOM
    Switch the javascript engine ON/OFF
  -----------------------------------------------------------------------*/
void SwitchJavaScript (Document document, View view)
{
#ifdef _JAVA
  Switch_JS_DOM (document, view);
#endif /* _JAVA */
}

/*----------------------------------------------------------------------
  ExecuteACommand
  Display a dialog box where the user can enter a javascript command
  -----------------------------------------------------------------------*/
void ExecuteACommand (Document document, View view)
{
#ifdef _JAVA
   Execute_ACommand (document, view);
#endif /* _JAVA */
}

/*----------------------------------------------------------------------
  InsertScript
  Add a <script> and open the structure View
  -----------------------------------------------------------------------*/
void InsertScript (Document document, View view)
{
  CreateScript (document, view, FALSE);
}

/*----------------------------------------------------------------------
  RemoveDeprecatedElements
  Remove <font>, <basefont>, <center>, <dir>, <menu>,
  <applet>, <isindex>, <s>, <u>, <strike>
  Remove lang and style attributes except on <span> elements
  -----------------------------------------------------------------------*/
void RemoveDeprecatedElements (Document doc, View view)
{
  Element             el, old, parent, child, root;
  ElementType         elType, searchedType1, searchedType2;
  ElementType         searchedType3, searchedType4, searchedType5;
  AttributeType       attrType, attrType1, attrType2;
  Attribute           attr;
  DisplayMode         dispMode;
  char               *s, message[500];
  int                 count = 0;
  ThotBool            modified = FALSE, closeUndo;
  
  /* get the insert point */
  root = TtaGetMainRoot (doc);
  el = root;
  if (el == NULL || TtaIsReadOnly (el))
    {
      /* no selection */
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
      return;
    }

  elType = TtaGetElementType (el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (s, "HTML"))
    {
      /* check if there is HTML Hi elements and if the current position is
         within a HTML Body element */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, SuspendDisplay);

      if (TtaHasUndoSequence (doc))
        closeUndo = FALSE;
      else
        {
          closeUndo = TRUE;
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
        }

      /* Don't check the Thot abstract tree against the structure schema. */
      TtaSetStructureChecking (FALSE, doc);

      /* remove <font> <basefont> <center> <dir> <menu> */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_TextAlign;
      attrType1.AttrSSchema = elType.ElSSchema;
      attrType2.AttrSSchema = elType.ElSSchema;
      searchedType1.ElSSchema = elType.ElSSchema;
      searchedType1.ElTypeNum = HTML_EL_Font_;
      searchedType2.ElSSchema = elType.ElSSchema;
      searchedType2.ElTypeNum = HTML_EL_Center;
      searchedType3.ElSSchema = elType.ElSSchema;
      searchedType3.ElTypeNum = HTML_EL_Directory;
      searchedType4.ElSSchema = elType.ElSSchema;
      searchedType4.ElTypeNum = HTML_EL_Menu;
      searchedType5.ElSSchema = elType.ElSSchema;
      searchedType5.ElTypeNum = HTML_EL_BaseFont;
      while (el)
        {
          el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                            searchedType3, searchedType4,
                                            searchedType5, SearchForward, el);
          if (el)
            {
              parent = TtaGetParent (el);
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == HTML_EL_Font_ ||
                  IsCharacterLevelElement (parent))
                {
                  old = el;
                  el = parent;
                  // move all children before the removed element
                  child = TtaGetFirstChild (old);
                  while (child)
                    {
                      TtaRegisterElementDelete (child, doc);
                      TtaRemoveTree (child, doc);
                      TtaInsertSibling (child, old, TRUE, doc);
                      TtaRegisterElementCreate (child, doc);
                      TtaNextSibling (&child);
                    }
                  count++;
                  TtaRegisterElementDelete (old, doc);
                  TtaDeleteTree (old, doc);
                  modified = TRUE;
                }
              else if (elType.ElTypeNum == HTML_EL_Center)
                {
                  // change to a Division
                  TtaChangeTypeOfElement (el, doc, HTML_EL_Division);
                  /* register the change in the undo sequence */
                  TtaRegisterElementTypeChange (el, HTML_EL_Division, doc);
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (el, attr, doc);
                      TtaSetAttributeValue (attr, HTML_ATTR_TextAlign_VAL_center_, el, doc);
                      TtaRegisterAttributeCreate (attr, el, doc);
                    }
                  count++;
                  modified = TRUE;
                }
              else
                {
                  // change to a Unnumbered_List
                  TtaChangeTypeOfElement (el, doc, HTML_EL_Unnumbered_List);
                  /* register the change in the undo sequence */
                  TtaRegisterElementTypeChange (el, HTML_EL_Unnumbered_List, doc);
                  count++;
                  modified = TRUE;
                }
            }
        }

      /* remove <font> <basefont> <center> <dir> <menu> */
      el = root;
      searchedType1.ElSSchema = elType.ElSSchema;
      searchedType1.ElTypeNum = HTML_EL_Applet;
      searchedType2.ElSSchema = elType.ElSSchema;
      searchedType2.ElTypeNum = HTML_EL_ISINDEX;
      searchedType3.ElSSchema = elType.ElSSchema;
      searchedType3.ElTypeNum = HTML_EL_Struck_text;
      searchedType4.ElSSchema = elType.ElSSchema;
      searchedType4.ElTypeNum = HTML_EL_Font;
      searchedType5.ElSSchema = elType.ElSSchema;
      searchedType5.ElTypeNum = HTML_EL_Invalid_element;
      while (el)
        {
          el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                            searchedType3, searchedType4,
                                            searchedType5, SearchForward, el);
          if (el)
            {
              parent = TtaGetParent (el);
              old = el;
              el = parent;
              // move all children before the removed element
              child = TtaGetFirstChild (old);
              while (child)
                {
                  TtaRegisterElementDelete (child, doc);
                  TtaRemoveTree (child, doc);
                  TtaInsertSibling (child, old, TRUE, doc);
                  TtaNextSibling (&child);
                  TtaRegisterElementCreate (child, doc);
                }
              TtaRegisterElementDelete (old, doc);
              TtaDeleteTree (old, doc);
              count++;
              modified = TRUE;
            }
        }

      /* remove language and style attributes */
      el = root;
      attrType1.AttrTypeNum = HTML_ATTR_Style_;
      attrType2.AttrTypeNum = HTML_ATTR_Language;
      while (el)
        {
          TtaSearchAttributes (attrType1,attrType2, SearchForward,
                               el, &el, &attr);
          if (el)
            {
              elType = TtaGetElementType (el);
              if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") ||
                  (elType.ElTypeNum != HTML_EL_Division &&
                   elType.ElTypeNum != HTML_EL_Table_ &&
                   elType.ElTypeNum != HTML_EL_IMG &&
                   elType.ElTypeNum != HTML_EL_COL &&
                   elType.ElTypeNum != HTML_EL_COLGROUP &&
                   elType.ElTypeNum != HTML_EL_Span))
                {
                  parent = TtaGetParent (el);
                  TtaRegisterAttributeDelete (attr, el, doc);
                  TtaRemoveAttribute (el, attr, doc);
                  el = parent;
                  count++;
                  modified = TRUE;
                }
            }
        }

      TtaSetStructureChecking (TRUE, doc);
      if (closeUndo)
        TtaCloseUndoSequence (doc);
      if (modified)
        TtaSetDocumentModified (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
      // display the result
      sprintf (message, TtaGetMessage (AMAYA, AM_CHANGE_NUMBER), count);
      ShowMessage (message, TtaGetMessage (AMAYA, AM_CHANGE));
    }
}

/*----------------------------------------------------------------------
  AddExternal
    Add a <script> inside with a src attribute pointing to
    an external js file.
  -----------------------------------------------------------------------*/
void AddExternal(Document document, View view)
{
  CreateScript (document, view, TRUE);
}

/*----------------------------------------------------------------------
  ExecuteExternal
  -----------------------------------------------------------------------*/
void ExecuteExternal (Document document, View view)
{
#ifdef _JAVA
   Execute_External (document, view);
#endif /* _JAVA */
}

/*----------------------------------------------------------------------
  SingleNewXHTML: Create a new XHTML document
  ----------------------------------------------------------------------*/
void SingleNewXHTML (Document doc, View view)
{
  OpenNew (doc, view, docHTML, L_Other);
}

/*----------------------------------------------------------------------
  DoNewXHTML: Create a new XHTML document
  ----------------------------------------------------------------------*/
void DoNewXHTML (Document doc, View view)
{
  OpenNew (doc, view, docHTML, 0);
}

/*----------------------------------------------------------------------
  NewXHTML: Create a new XHTML document
  ----------------------------------------------------------------------*/
void NewXHTML (Document doc, View view)
{
  OpenNew (doc, view, docHTML, L_Other);
}

/*----------------------------------------------------------------------
  NewLibrary: Create a new  document
  ----------------------------------------------------------------------*/
void NewLibrary (Document doc, View view)
{
  OpenNew (doc, view, docTemplate, L_Other);
}

/*----------------------------------------------------------------------
  NewMathML: Create a new MathML document
  ----------------------------------------------------------------------*/
void NewMathML (Document doc, View view)
{
  OpenNew (doc, view, docMath, L_MathML);
}
/*----------------------------------------------------------------------
  NewSVG: Create a new XHTML document
  ----------------------------------------------------------------------*/
void NewSVG (Document doc, View view)
{
  OpenNew (doc, view, docSVG, L_SVG);
}

/*----------------------------------------------------------------------
  NewCss: Create a new CSS stylesheet
  ----------------------------------------------------------------------*/
void NewCss (Document doc, View view)
{
  OpenNew (doc, view, docCSS, L_CSS);
}

/*--------------------------------------------------------------------------
  AddRDFaNS
  Add the RDFa specific namespace declarations to the root element
  --------------------------------------------------------------------------*/
static void AddRDFaNS (Document doc)
{
  ElementType      elType;
  Element          docEl, root;
  char            *path, *tempPath, *homePath, *configPath, *ptr;
  unsigned char    c;
  FILE            *file;
  char             line[MAX_LENGTH], prefix[MAX_LENGTH], uri[MAX_LENGTH];
  int              len;
  ThotBool         pref;

  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);
  elType.ElTypeNum = HTML_EL_HTML;
  root = TtaSearchTypedElement (elType, SearchInTree, docEl);
   if (root)
    {
      // XHTML namespace
      TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);

      // Open the RDFa config file
      path = (char *) TtaGetMemory (MAX_LENGTH);
      configPath = (char *) TtaGetMemory (MAX_LENGTH);
      tempPath = (char *) TtaGetMemory (MAX_LENGTH);
      homePath = TtaGetEnvString ("APP_HOME");
      sprintf (path, "%s%crdfa.dat", homePath, DIR_SEP);
      sprintf (tempPath, "%s%crdfa-tmp.dat", homePath, DIR_SEP);
      file = TtaReadOpen ((char *)path);
      if (!file)
        {
          // open the default file      
          ptr = TtaGetEnvString ("THOTDIR");
          strncpy (configPath, ptr, MAX_LENGTH - 20);
          configPath[MAX_LENGTH - 20] = EOS;
          strcat (configPath, DIR_STR);
          strcat (configPath, "config");
          strcat (configPath, DIR_STR);
          strcat (configPath, "rdfa.dat");
          file = TtaReadOpen ((char *)configPath);
          if (!file)
            {
              /* The config file doesn't exist, load a static configuration */
              file = TtaWriteOpen ((char *)tempPath);
              fprintf (file, "rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n");
              fprintf (file, "rdfs=\"http://www.w3.org/2000/01/rdf-schema#\"\n");
              fprintf (file, "owl=\"http://www.w3.org/2002/07/owl#\"\n");
              fprintf (file, "xsd=\"http://www.w3.org/2001/XMLSchema#\"\n");
              fprintf (file, "foaf=\"http://xmlns.com/foaf/0.1/\"\n");
              fprintf (file, "dc=\"http://purl.org/dc/elements/1.1/\"\n");
              TtaWriteClose (file);
              /* Retry to open it */
              file = TtaReadOpen ((char *)tempPath);
            }
        }
      
      if (file)
        {
          prefix[0] = EOS;
          uri[0] = EOS;
          line[0] = EOS;
          len = 0;
          pref = FALSE;

          while (len < MAX_LENGTH && TtaReadByte (file, &c))
            {
              if (c == 13 || c == EOL)
                {
                  line[len] = EOS;
                  strcpy (uri, line);
                  line[0] = EOS;
                  len = 0;
                  if (prefix[0] != EOS && uri[0] != EOS)
                    {
                      TtaSetANamespaceDeclaration (doc, root, prefix, uri);
                      prefix[0] = EOS;
                      uri[0] = EOS;
                      pref = FALSE;
                    }
                }
              else if (c == '=')
                {
                  line[len] = EOS;
                  strcpy (prefix, line);
                  pref = TRUE;
                  line[0] = EOS;
                  len = 0;
                }
              else
                {
                  if (c != '"')
                    line[len++] = (char)c;
                }
            }
          TtaReadClose (file);
        }
      TtaFreeMemory(path);
      TtaFreeMemory(configPath);
      TtaFreeMemory(tempPath);
    }
}

/*--------------------------------------------------------------------------
  CreateDoctype creates a doctype declaration
  The parameter doctype points to the current DOCTYPE or NULL.
  Parameters useMathML and useSVG determine the XHTML 1.1 profile.
  --------------------------------------------------------------------------*/
void CreateDoctype (Document doc, Element doctype, int profile, int extraProfile,
                    ThotBool useMathML, ThotBool useSVG, ThotBool newDoc)
{
  ElementType     elType, lineType, piType;
  Element         docEl, doctypeLine, text, child, prev;
  Language        language;
  char		  buffer[400], *name, *private_dtd;
  
  /* Don't check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (FALSE, doc);
  
  /* We use the Latin_Script language to avoid */
  /* the spell_chekcer to check the doctype */
  language = Latin_Script;
  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);
  lineType.ElSSchema = elType.ElSSchema;
  lineType.ElTypeNum = 0;
  piType.ElSSchema = elType.ElSSchema;
  piType.ElTypeNum = 0;
  name = TtaGetSSchemaName (elType.ElSSchema);
  /* Add the new doctype */
  if (profile == L_Basic || profile == L_Strict ||
      profile == L_Xhtml11 || profile == L_Transitional)
    {
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      lineType.ElTypeNum = HTML_EL_DOCTYPE_line;
      piType.ElTypeNum = HTML_EL_XMLPI;
    }
#ifdef _SVG
  else if (profile == L_SVG) 
    {
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      lineType.ElTypeNum = SVG_EL_DOCTYPE_line;
      piType.ElTypeNum = HTML_EL_XMLPI;
    }
#endif /* _SVG */
  else if (profile == L_MathML) 
    {
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      lineType.ElTypeNum = MathML_EL_DOCTYPE_line;
      piType.ElTypeNum = MathML_EL_XMLPI;
    }

  if (doctype == NULL)
    {
      /* no DOCTYPE already declared */
      doctype = TtaNewElement (doc, elType);
      /* skip PI */
      child = TtaGetFirstChild (docEl);
      if (child == NULL)
        TtaInsertFirstChild (&doctype, docEl, doc);
      else
        while (child)
          {
            elType = TtaGetElementType (child);
            if (piType.ElTypeNum == elType.ElTypeNum &&
                piType.ElSSchema == elType.ElSSchema)
              {
                /* it's a PI */
                prev = child;
                TtaNextSibling (&child);
                if (child == NULL)
                  TtaInsertSibling (doctype, prev, FALSE, doc);
              }
            else
              {
                /* insert before this child */
                TtaInsertSibling (doctype, child, TRUE, doc);
                child = NULL;
              }
          }
      /* Make the DOCTYPE element read-only */
      TtaSetAccessRight (doctype, ReadOnly, doc);
    }
  else
    {
      doctypeLine = TtaGetFirstChild (doctype);
      while (doctypeLine)
        {
          TtaDeleteTree (doctypeLine, doc);
          doctypeLine = TtaGetFirstChild (doctype);
        }
    }
  /* Create the first DOCTYPE_line element */
  elType.ElTypeNum = lineType.ElTypeNum;
  doctypeLine = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&doctypeLine, doctype, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, doc);
      // check first if the user wants to use a private HTML declaration
      private_dtd = TtaGetEnvString ("LOCAL_HTML_DOCTYPE_1");
      if (private_dtd && private_dtd[0] != EOS)
        TtaSetTextContent (text, (unsigned char*)private_dtd, language, doc);
      else if (profile == L_Basic)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML10_BASIC, language, doc);
      else if (profile == L_Strict && DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML10_STRICT, language, doc);
      else if (profile == L_Strict && !DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_HTML_STRICT, language, doc);
      else if (profile == L_Xhtml11 && useMathML && !useSVG)
        {
          strcpy (buffer, DOCTYPE1_XHTML11_PLUS_MATHML);
          TtaSetTextContent (text, (unsigned char*)buffer, language, doc);
        }
      else if (profile == L_Xhtml11 && useSVG)
        {
          if (useMathML)
            {
              strcpy (buffer, DOCTYPE1_XHTML11_PLUS_MATHML_PLUS_SVG);
              TtaSetTextContent (text, (unsigned char*)buffer, language, doc);
            }
          else
            TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML11_PLUS_MATHML_PLUS_SVG,
                               language, doc);
        }
      else if ((profile == L_Xhtml11) && (extraProfile == L_RDFa))
	{
	  if (newDoc)
	    AddRDFaNS (doc);
	  TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML_PLUS_RDFa, language, doc);
	}
      else if (profile == L_Xhtml11)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML11, language, doc);
      else if (profile == L_Transitional && DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_XHTML10_TRANSITIONAL, language, doc);
      else if (profile == L_Transitional && !DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_HTML_TRANSITIONAL, language, doc);
      else if (profile == L_MathML)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_MATHML20, language, doc);
      else if (profile == L_SVG)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE1_SVG10, language, doc);
    }
  
  /* Create the second DOCTYPE_line element */
  elType.ElTypeNum = lineType.ElTypeNum;
  doctypeLine = TtaNewElement (doc, elType);
  child = TtaGetLastChild (doctype);
  TtaInsertSibling (doctypeLine, child, FALSE, doc);
  elType.ElTypeNum = 1;
  text = TtaNewElement (doc, elType);
  if (text != NULL)
    {
      TtaInsertFirstChild (&text, doctypeLine, doc);
      // check first if the user wants to use a private HTML declaration
      private_dtd = TtaGetEnvString ("LOCAL_HTML_DOCTYPE_2");
      if (private_dtd && private_dtd[0] != EOS)
        TtaSetTextContent (text, (unsigned char*)private_dtd, language, doc);
      else if (profile == L_Basic)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML10_BASIC, language, doc);
      else if (profile == L_Strict && DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML10_STRICT, language, doc);
      else if (profile == L_Strict && !DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_HTML_STRICT, language, doc);
      else if (profile == L_Xhtml11 && useMathML && !useSVG)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML11_PLUS_MATHML, language, doc);
      else if (profile == L_Xhtml11 && useSVG)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML11_PLUS_MATHML_PLUS_SVG, language,
                           doc);
      else if ((profile == L_Xhtml11) && (extraProfile == L_RDFa))
	{
	  if (newDoc)
	    AddRDFaNS (doc);
	  TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML_PLUS_RDFa, language, doc);
	}
      else if (profile == L_Xhtml11)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML11, language, doc);
      else if (profile == L_Transitional && DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_XHTML10_TRANSITIONAL, language, doc);
      else if (profile == L_Transitional && !DocumentMeta[doc]->xmlformat)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_HTML_TRANSITIONAL, language, doc);
      else if (profile == L_MathML)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_MATHML20, language, doc);
      else if (profile == L_SVG)
        TtaSetTextContent (text, (unsigned char*)DOCTYPE2_SVG10, language, doc);
    }
  TtaSetStructureChecking (TRUE, doc);
}

/*----------------------------------------------------------------------
  InitializeNewDoc builds the initial contents of a new document
  When the parameter doc is 0 the function creates a new document window.
  The url is coded with the default charset.
  ----------------------------------------------------------------------*/
void InitializeNewDoc (char *url, int docType, Document doc, int profile,
                       int extraProfile, ThotBool isXML)
{
  ElementType          elType;
  Element              docEl, root, title, text, el, head, child, meta, body;
  Element              doctype;
  AttributeType        attrType;
  Attribute            attr;
  Language             language;
  char                *pathname, *documentname;
  char                *s;
  char                 tempfile[MAX_LENGTH];
  char                *charsetName;
  CHARSET              charset;
  ThotBool             xhtml_mimetype;

  if (!IsW3Path (url) && TtaFileExist (url))
    {
      s = (char *)TtaGetMemory (strlen (url) +
                                strlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK)) + 2);
      sprintf (s, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), url);
      InitConfirm (0, 0, s);
      TtaFreeMemory (s);
      if (!UserAnswer)
        return;
    }

  pathname = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);
  NormalizeURL (url, 0, pathname, documentname, NULL);
  if (doc == 0 || DontReplaceOldDoc)
    {
      doc = InitDocAndView (doc, !DontReplaceOldDoc, InNewWindow,
                            documentname, (DocumentType)docType, 0, FALSE, profile, extraProfile,
                            CE_ABSOLUTE);
      InitDocHistory (doc);
      DontReplaceOldDoc = FALSE;
    }
  else
    {
      /* record the current position in the history */
      AddDocHistory (doc, DocumentURLs[doc], 
                     DocumentMeta[doc]->initial_url,
                     DocumentMeta[doc]->form_data,
                     DocumentMeta[doc]->method);
      doc = InitDocAndView (doc,
                            !DontReplaceOldDoc, InNewWindow,
                            documentname, (DocumentType)docType, 0, FALSE, profile, extraProfile,
                            CE_ABSOLUTE);
    }
  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaStrdup (url);
  DocumentURLs[doc] = s;
  AddURLInCombobox (url, NULL, TRUE);
  TtaSetTextZone (doc, 1, URL_list);
  DocumentMeta[doc] = DocumentMetaDataAlloc ();
  DocumentMeta[doc]->form_data = NULL;
  DocumentMeta[doc]->initial_url = NULL;
  DocumentMeta[doc]->method = CE_ABSOLUTE;
  DocumentMeta[doc]->xmlformat = FALSE;
  DocumentMeta[doc]->compound = FALSE;
  DocumentSource[doc] = 0;

  /* store the document profile */
  TtaSetDocumentProfile (doc, profile, extraProfile);

  ResetStop (doc);
  language = TtaGetDefaultLanguage ();
  docEl = TtaGetMainRoot (doc);
  /* Set the document charset */
  charsetName = TtaGetEnvString ("DOCUMENT_CHARSET");
  charset = TtaGetCharset (charsetName);
  if (charset != UNDEFINED_CHARSET)
    {
      TtaSetDocumentCharset (doc, charset, FALSE);
      DocumentMeta[doc]->charset = TtaStrdup (charsetName);
    }
  else
    {
      TtaSetDocumentCharset (doc, ISO_8859_1, FALSE);
      DocumentMeta[doc]->charset = TtaStrdup ("iso-8859-1");
    }

  elType = TtaGetElementType (docEl);
  attrType.AttrSSchema = elType.ElSSchema;

  if (docType == docHTML)
    {
      /*-------------  New XHTML document ------------*/
      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = isXML;
      DocumentMeta[doc]->compound = FALSE;
      TtaGetEnvBoolean ("ENABLE_XHTML_MIMETYPE", &xhtml_mimetype);
      if (xhtml_mimetype)
        DocumentMeta[doc]->content_type = TtaStrdup (AM_XHTML_MIME_TYPE);
      else
        DocumentMeta[doc]->content_type = TtaStrdup ("text/html");

      /* create the DOCTYPE element corresponding to the document's profile */
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (profile != L_Other)
        CreateDoctype (doc, doctype, profile, extraProfile, FALSE, FALSE, TRUE);
      else if (doctype)
        TtaDeleteTree (doctype, doc);
      
      /* Load user's style sheet */
      LoadUserStyleSheet (doc);

      /* Set the namespace declaration */
      elType.ElTypeNum = HTML_EL_HTML;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);
      TtaSetUriSSchema (elType.ElSSchema, XHTML_URI);

      /* attach an attribute PrintURL to the root element */
      attrType.AttrTypeNum = HTML_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);

      /* create a default title if there is no content in the TITLE element */
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchInTree, root);
      text = TtaGetFirstChild (title);
      if (TtaGetTextLength (text) == 0)
        {
          if (Answer_text[0] == EOS)
            TtaSetTextContent (text, (unsigned char*)"No title", language, doc);
          else
            TtaSetTextContent (text, (unsigned char*)Answer_text, language, doc);
        }
      UpdateTitle (title, doc);

      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchInTree, root);

      /* create a Document_URL element as the first child of HEAD */
      elType.ElTypeNum = HTML_EL_Document_URL;
      el = TtaSearchTypedElement (elType, SearchInTree, head);
      if (el == NULL)
        {
          /* there is no Document_URL element, create one */
          el = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&el, head, doc);
        }
      /* prevent the user from editing this element */
      TtaSetAccessRight (el, ReadOnly, doc);
      /* element Document_URL already exists */
      text = TtaGetFirstChild (el);
      if (text == NULL)
        {
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          text = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&text, el, doc);
        }
      if (url != NULL && text != NULL)
        TtaSetTextContent (text, (unsigned char*)url, language, doc);

      /* create a META element in the HEAD with name="generator" */
      /* and content="Amaya" */
      child = TtaGetLastChild (head);
      elType.ElTypeNum = HTML_EL_META;
      meta = TtaNewElement (doc, elType);
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      TtaSetAttributeText (attr, "generator", meta, doc);
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      strcpy (tempfile, "Amaya");
      strcat (tempfile, ", see http://www.w3.org/Amaya/");
      TtaSetAttributeText (attr, tempfile, meta, doc);
      TtaInsertSibling (meta, child, FALSE, doc);

      /* create a BODY element if there is not */
      elType.ElTypeNum = HTML_EL_BODY;
      body = TtaSearchTypedElement (elType, SearchInTree, root);
      if (!body)
        {
          body = TtaNewTree (doc, elType, "");
          TtaInsertSibling (body, head, FALSE, doc);
        }

      /* Search the first element in the BODY to set initial selection */
      elType.ElTypeNum = HTML_EL_Element;
      el = TtaSearchTypedElement (elType, SearchInTree, body);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
        UpdateContextSensitiveMenus (SelectionDoc, 1);
      SelectionDoc = doc;
      /* Activate show areas */
      if (MapAreas[doc])
        ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
      TtaRaiseDoctypePanels (WXAMAYA_DOCTYPE_XHTML);
    }
  else if (docType == docMath)
    {
      /*-------------  New MathML document ------------*/
      /* Set the IntDisplaystyle attribute */
      elType.ElTypeNum = MathML_EL_MathML;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (root)
        SetDisplaystyleMathElement (root, doc);

      /* create the MathML DOCTYPE element */
      elType.ElTypeNum = MathML_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      CreateDoctype (doc, doctype, L_MathML, 0, FALSE, FALSE, TRUE);

      /* Set the namespace declaration */
      root = TtaGetRootElement (doc);
      TtaSetUriSSchema (elType.ElSSchema, MathML_URI);
      TtaSetANamespaceDeclaration (doc, root, NULL, MathML_URI);

      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      DocumentMeta[doc]->compound = FALSE;
      /* Search the first Construct to set the initial selection */
      elType.ElTypeNum = MathML_EL_Construct;
      el = TtaSearchTypedElement (elType, SearchInTree, docEl);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
        UpdateContextSensitiveMenus (SelectionDoc, 1);
      SelectionDoc = doc;
      TtaRaiseDoctypePanels(WXAMAYA_DOCTYPE_MATHML);
    }
  else if (docType == docSVG)
    {
#ifdef _SVG
      /*-------------  New SVG document ------------*/
      /* create the SVG DOCTYPE element */
      elType.ElTypeNum = SVG_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      CreateDoctype (doc, doctype, L_SVG, 0, FALSE, FALSE, TRUE);

      /* Set the namespace declaration */
      root = TtaGetRootElement (doc);
      TtaSetUriSSchema (elType.ElSSchema, SVG_URI);
      TtaSetANamespaceDeclaration (doc, root, NULL, SVG_URI);

      /* Set the version attribute */
      elType.ElTypeNum = SVG_EL_SVG;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (root)
        {
          // set the new Coordinate System
          TtaSetElCoordinateSystem (root);
          attrType.AttrTypeNum = SVG_ATTR_version;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (root, attr, doc);
          TtaSetAttributeText (attr, SVG_VERSION, root, doc);

          // fix the width and height
          attrType.AttrTypeNum = SVG_ATTR_width_;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (root, attr, doc);
          TtaSetAttributeText (attr, "600", root, doc);
          ParseWidthHeightAttribute (attr, root, doc, FALSE);

          attrType.AttrTypeNum = SVG_ATTR_height_;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (root, attr, doc);
          TtaSetAttributeText (attr, "600", root, doc);
          ParseWidthHeightAttribute (attr, root, doc, FALSE);

          /* create a default title if there is no title element */
          elType.ElTypeNum = SVG_EL_title;
          title = TtaSearchTypedElement (elType, SearchInTree, root);
          if (!title)
            {
              title = TtaNewTree (doc, elType, "");
              TtaInsertFirstChild (&title, root, doc);
            }
          text = TtaGetFirstChild (title);
          if (text && TtaGetTextLength (text) == 0)
            TtaSetTextContent (text, (unsigned char*)"Draw", language,doc);
          UpdateTitle (title, doc);
          TtaRaiseDoctypePanels(WXAMAYA_DOCTYPE_SVG);
        }

      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      DocumentMeta[doc]->compound = FALSE;
      /* Search the last element to set the initial selection */
      el = TtaGetLastLeaf (docEl);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
        UpdateContextSensitiveMenus (SelectionDoc, 1);
      SelectionDoc = doc;
#endif /* _SVG */
    }
  else if (docType == docTemplate)
    {
#ifdef TEMPLATES
      /*-------------  New Template library document ------------*/
      /* Set the namespace declaration */
      root = TtaGetRootElement (doc);
      TtaSetUriSSchema (elType.ElSSchema, Template_URI);
      TtaSetANamespaceDeclaration (doc, root, "xt", Template_URI);
      SetAttributeStringValue(root, Template_ATTR_version, Template_Current_Version);
      SetAttributeStringValue(root, Template_ATTR_templateVersion, "1.0");
      TtaNewNature (doc, elType.ElSSchema,  NULL, "HTML", "HTMLP");
      TtaSetANamespaceDeclaration (doc, root, "ht", XHTML_URI);

      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      DocumentMeta[doc]->compound = TRUE;
      Template_PrepareLibrary (url, doc, "1.0");
      UpdateTemplateMenus (doc);
      /* set the initial selection */
      el = TtaGetLastChild (root);
      if (el)
         TtaSelectElement (doc, el);
      if (SelectionDoc != 0)
        UpdateContextSensitiveMenus (SelectionDoc, 1);
      SelectionDoc = doc;
      TtaRaiseDoctypePanels (WXAMAYA_DOCTYPE_XTIGER);
#endif /* _TEMPLATES */
    }
  else
    {
      /*-------------  Other documents ------------*/
      elType.ElTypeNum = TextFile_EL_TextFile;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);

      if (docType == docCSS)
        {
          /* add the attribute Source */
          attrType.AttrTypeNum = TextFile_ATTR_Source;
          attr = TtaGetAttribute (root, attrType);
          if (attr == 0)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (root, attr, doc);
            }
        }

      /* attach the default attribute PrintURL to the root element */
      attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);
      /* insert the Document_URL element */
      el = TtaGetFirstChild (root);
      if (el == NULL)
        {
          elType.ElTypeNum = TextFile_EL_Document_URL;
          el = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&el, root, doc);
        }
      /* prevent the user from editing this element */
      TtaSetAccessRight (el, ReadOnly, doc);
      el = TtaGetFirstChild (el);     
      TtaSetTextContent (el, (unsigned char*)url, language, doc);

      body = TtaGetLastChild (root);
      /* get the line */
      el = TtaGetLastChild (body);
      /* and the empty text within this line */
      el = TtaGetLastChild (el);
      /* set the initial selection */
      TtaSelectElement (doc, el);
      SelectionDoc = doc;
    }

  /* the document should be saved */
  TtaSetDocumentUnmodified (doc);
#ifdef _WX
  // set the default icon
  TtaSetPageIcon (doc, 1, NULL);
  UpdateStyleList (doc, 1);
#endif /* _WX */
  UpdateEditorMenus (doc);
  UpdateContextSensitiveMenus (doc, 1);
  TtaRefreshActiveFrame ();
}

/*----------------------------------------------------------------------
  NotFoundDoc builds the 404 error document
  When the parameter doc is 0 the function creates a new document window.
  The url is coded with the default charset.
  ----------------------------------------------------------------------*/
void NotFoundDoc (char *url, Document doc)
{
  ElementType          elType;
  Element              docEl, root, title, text, el, head, child, meta, body;
  Element              doctype;
  AttributeType        attrType;
  Attribute            attr;
  Language             language;
  char                *pathname, *documentname;
  char                *s;
  char                 tempfile[MAX_LENGTH];
  char                *charsetName;
  CHARSET              charset;
  ThotBool             xhtml_mimetype, empty;

  pathname = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);
  // check if the user wants to open an empty document
#ifdef _WINDOWS
  sprintf (pathname, "%s\\empty", TtaGetEnvString ("THOTDIR"));
#else /* _WINDOWS */
  sprintf (pathname, "%s/empty", TtaGetEnvString ("THOTDIR"));
#endif /* _WINDOWS */
  NormalizeFile (pathname, tempfile, AM_CONV_NONE);
  empty = !strcmp (tempfile, url);

  NormalizeURL (url, 0, pathname, documentname, NULL);
  if (empty)
      strcpy (url, "empty");

  if (doc == 0 || DontReplaceOldDoc)
    {
      doc = InitDocAndView (doc,
                            !DontReplaceOldDoc /* replaceOldDoc */,
                            InNewWindow /* inNewWindow */,
                            documentname, docHTML, 0, FALSE, L_Strict, 0,
                            CE_ABSOLUTE);
      InitDocHistory (doc);
      DontReplaceOldDoc = FALSE;
    }
  else
    {
      /* record the current position in the history */
      AddDocHistory (doc, DocumentURLs[doc], 
                     DocumentMeta[doc]->initial_url,
                     DocumentMeta[doc]->form_data,
                     DocumentMeta[doc]->method);
      doc = InitDocAndView (doc,
                            !DontReplaceOldDoc /* replaceOldDoc */,
                            InNewWindow /* inNewWindow */,
                            documentname, docHTML, 0, FALSE, L_Strict, 0,
                            CE_ABSOLUTE);
    }
  TtaFreeMemory (documentname);
  TtaFreeMemory (pathname);

  /* save the document name into the document table */
  s = TtaStrdup (url);
  DocumentURLs[doc] = s;
  AddURLInCombobox (url, NULL, FALSE);
  TtaSetTextZone (doc, 1, URL_list);
  DocumentMeta[doc] = DocumentMetaDataAlloc ();
  DocumentMeta[doc]->form_data = NULL;
  DocumentMeta[doc]->initial_url = NULL;
  DocumentMeta[doc]->method = CE_ABSOLUTE;
  DocumentMeta[doc]->xmlformat = FALSE;
  DocumentMeta[doc]->compound = FALSE;
  DocumentSource[doc] = 0;

  /* store the document profile */
  TtaSetDocumentProfile (doc, L_Strict, 0);
  ResetStop (doc);
  language = TtaGetDefaultLanguage ();
  docEl = TtaGetMainRoot (doc);
  /* Set the document charset */
  charsetName = TtaGetEnvString ("DOCUMENT_CHARSET");
  charset = TtaGetCharset (charsetName);
  if (charset != UNDEFINED_CHARSET)
    {
      TtaSetDocumentCharset (doc, charset, FALSE);
      DocumentMeta[doc]->charset = TtaStrdup (charsetName);
    }
  else
    {
      TtaSetDocumentCharset (doc, ISO_8859_1, FALSE);
      DocumentMeta[doc]->charset = TtaStrdup ("iso-8859-1");
    }

  if (empty)
    root = docEl;
  else
    {
      elType = TtaGetElementType (docEl);
      attrType.AttrSSchema = elType.ElSSchema;

      /*-------------  New XHTML document ------------*/
      /* force the XML parsing */
      DocumentMeta[doc]->xmlformat = TRUE;
      DocumentMeta[doc]->compound = FALSE;
      TtaGetEnvBoolean ("ENABLE_XHTML_MIMETYPE", &xhtml_mimetype);
      if (xhtml_mimetype)
        DocumentMeta[doc]->content_type = TtaStrdup (AM_XHTML_MIME_TYPE);
      else
        DocumentMeta[doc]->content_type = TtaStrdup ("text/html");

      /* create the DOCTYPE element corresponding to the document's profile */
      elType.ElTypeNum = HTML_EL_DOCTYPE;
      doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (doctype)
        TtaDeleteTree (doctype, doc);
      /* Load user's style sheet */
      LoadUserStyleSheet (doc);

      /* Set the namespace declaration */
      elType.ElTypeNum = HTML_EL_HTML;
      root = TtaSearchTypedElement (elType, SearchInTree, docEl);
      TtaSetUriSSchema (elType.ElSSchema, XHTML_URI);
      TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);

      /* attach an attribute PrintURL to the root element */
      attrType.AttrTypeNum = HTML_ATTR_PrintURL;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (root, attr, doc);

      s = (char *)TtaConvertByteToMbs ((unsigned char *)TtaGetMessage (LIB, TMSG_NOT_FOUND),
                                       TtaGetDefaultCharset ());
      /* create a default title if there is no content in the TITLE element */
      elType.ElTypeNum = HTML_EL_TITLE;
      title = TtaSearchTypedElement (elType, SearchInTree, root);
      text = TtaGetFirstChild (title);
      if (text && TtaGetTextLength (text) == 0)
        TtaSetTextContent (text, (unsigned char*)s, language, doc);
      UpdateTitle (title, doc);

      elType.ElTypeNum = HTML_EL_HEAD;
      head = TtaSearchTypedElement (elType, SearchInTree, root);

      /* create a Document_URL element as the first child of HEAD */
      elType.ElTypeNum = HTML_EL_Document_URL;
      el = TtaSearchTypedElement (elType, SearchInTree, head);
      if (el == NULL)
        {
          /* there is no Document_URL element, create one */
          el = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&el, head, doc);
        }
      /* prevent the user from editing this element */
      TtaSetAccessRight (el, ReadOnly, doc);
      /* element Document_URL already exists */
      text = TtaGetFirstChild (el);
      if (text == NULL)
        {
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          text = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&text, el, doc);
        }
      if (url && text)
        TtaSetTextContent (text, (unsigned char*)url, language, doc);

      /* create a META element in the HEAD with name="generator" */
      /* and content="Amaya" */
      child = TtaGetLastChild (head);
      elType.ElTypeNum = HTML_EL_META;
      meta = TtaNewElement (doc, elType);
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      TtaSetAttributeText (attr, "generator", meta, doc);
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, doc);
      strcpy (tempfile, TtaGetAppName());
      strcat (tempfile, " ");
      strcat (tempfile, TtaGetAppVersion());
      strcat (tempfile, ", see http://www.w3.org/Amaya/");
      TtaSetAttributeText (attr, tempfile, meta, doc);
      TtaInsertSibling (meta, child, FALSE, doc);

      /* create a BODY element if there is not */
      elType.ElTypeNum = HTML_EL_BODY;
      body = TtaSearchTypedElement (elType, SearchInTree, root);
      if (!body)
        {
          body = TtaNewTree (doc, elType, "");
          TtaInsertSibling (body, head, FALSE, doc);
        }

      /* Search the first element in the BODY to set initial selection */
      elType.ElTypeNum = HTML_EL_Element;
      el = TtaSearchTypedElement (elType, SearchInTree, body);
      /* Create a H1 */
      elType.ElTypeNum = HTML_EL_H1;
      child = TtaNewElement (doc, elType);
      TtaInsertSibling (child, el, TRUE, doc);
      /* Create a text */
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      text = TtaNewElement (doc, elType);
      TtaSetTextContent (text, (unsigned char*)s, language, doc);
      TtaInsertFirstChild (&text, child, doc);
      /* Create a paragraph */
      elType.ElTypeNum = HTML_EL_Paragraph;
      child = TtaNewElement (doc, elType);
      TtaInsertSibling (child, el, TRUE, doc);
      /* Create a text */
      TtaFreeMemory (s);
      s = (char *)TtaConvertByteToMbs ((unsigned char *)TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                                       TtaGetDefaultCharset ());
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      text = TtaNewElement (doc, elType);
      if (s)
        {
          pathname = (char *)TtaGetMemory (strlen (s) + strlen (url) + 1);
          sprintf (pathname, s, url);
          TtaSetTextContent (text, (unsigned char*)pathname, language, doc);
          TtaFreeMemory (pathname);
          TtaFreeMemory (s);
        }
      TtaInsertFirstChild (&text, child, doc);
      
      /* set the initial selection */
      UpdateContextSensitiveMenus (doc, 1);
      /* Activate show areas */
      if (MapAreas[doc])
        ChangeAttrOnRoot (doc, HTML_ATTR_ShowAreas);
    }
  /* Update the Doctype menu */
  UpdateDoctypeMenu (doc);
  /* the document should be saved */
  TtaSetDocumentUnmodified (doc);
  UpdateStyleList (doc, 1);
  UpdateEditorMenus (doc);
  TtaSetAccessRight (root, ReadOnly, doc);
}

/*--------------------------------------------------------------------------
  CreateOrChangeDoctype
  Create or change the doctype of a document
  --------------------------------------------------------------------------*/
static void CreateOrChangeDoctype (Document doc, View view, int new_doctype,
				   int new_extraProfile, ThotBool xmlDoctype,
				   ThotBool useMathML, ThotBool useSVG)
{
  char           *tempdoc = NULL; 
  char            documentname[MAX_LENGTH];
  char            tempdir[MAX_LENGTH];
  int             oldprofile, oldExtraprofile;
  ThotBool        ok = FALSE, error = FALSE;
  
  /* The document has to be parsed with the new doctype */
  tempdoc = GetLocalPath (doc, DocumentURLs[doc]);
  if (TtaIsDocumentUpdated (doc) || !TtaFileExist (tempdoc) ||
      (!DocumentMeta[doc]->xmlformat && xmlDoctype))
    {
      /* save the current state of the document */
      if (DocumentTypes[doc] == docLibrary || DocumentTypes[doc] == docHTML)
        {
          if (TtaGetDocumentProfile (doc) == L_Xhtml11 || TtaGetDocumentProfile (doc) == L_Basic)
            TtaExportDocumentWithNewLineNumbers (doc, tempdoc, "HTMLT11", FALSE);
          else if (DocumentMeta[doc]->xmlformat || xmlDoctype)
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
    }

  /* Parse the document with the new doctype */
  TtaExtractName (tempdoc, tempdir, documentname);
  /* change the document profile */
  oldprofile = TtaGetDocumentProfile (doc);
  oldExtraprofile = TtaGetDocumentExtraProfile (doc);
  ok = ParseWithNewDoctype (doc, tempdoc, tempdir, documentname, new_doctype,
                            new_extraProfile, &error, xmlDoctype, useMathML, useSVG);

  if (ok)
    {
      /* Update the Doctype menu */
      UpdateDoctypeMenu (doc);
      /* Notify the document as modified */
      TtaSetDocumentModified (doc);
      /* Reparse the document to remove the errors */
      if (error)
        {
          RestartParser (doc, tempdoc, tempdir, documentname, FALSE, FALSE);
          TtaSetDocumentModified (doc);
        }
      TtaSetDocumentUnupdated (doc);
    }
  else
    /* restore the document profile */
    TtaSetDocumentProfile (doc, oldprofile, oldExtraprofile);

  TtaFreeMemory (tempdoc);
}

/*--------------------------------------------------------------------------
  RemoveDoctype
  Remove the doctype declaration
  --------------------------------------------------------------------------*/
void RemoveDoctype (Document doc, View view)
{

  ElementType     elType;
  Element         docEl, doctype;
  char           *s;

  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);

  /* Search the doctype declaration according to the main schema */
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML") == 0)
    elType.ElTypeNum = HTML_EL_DOCTYPE;
  else if (strcmp (s, "SVG") == 0)
    elType.ElTypeNum = SVG_EL_DOCTYPE;
  else if (strcmp (s, "MathML") == 0)
    elType.ElTypeNum = MathML_EL_DOCTYPE;
  else
    elType.ElTypeNum = XML_EL_doctype;
  doctype = TtaSearchTypedElement (elType, SearchInTree, docEl);

  /* remove the existing doctype */
  if (doctype != NULL)
    {
      TtaDeleteTree (doctype, doc);
      TtaSetDocumentProfile (doc, L_Other, L_NoExtraProfile);
      UpdateDoctypeMenu (doc);
      TtaSetDocumentModified (doc);
    }
  /* Synchronize the document */
  Synchronize (doc, view);
}

/*--------------------------------------------------------------------------
  AddDoctype
  Add the doctype declaration
  --------------------------------------------------------------------------*/
void AddDoctype (Document doc, View view)
{

  DocumentType    docType;
  int             profile;
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  profile =  L_Other;
  docType = DocumentTypes[doc];
  if (docType == docHTML)
    {
      if (useMathML || useSVG)
        profile = L_Xhtml11;
      else
        profile = L_Transitional;
    }
  else if (docType == docMath)
    profile = L_MathML;
  else if (docType == docSVG)
    profile = L_SVG;

  CreateOrChangeDoctype (doc, view, profile, 0,
			 DocumentMeta[doc]->xmlformat, useMathML, useSVG);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeXhtml11
  Create or change the doctype for a XHTML 1.1 document
  --------------------------------------------------------------------------*/
void CreateDoctypeXhtml11 (Document doc, View view)
{
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  CreateOrChangeDoctype (doc, view, L_Xhtml11, 0, TRUE, useMathML, useSVG);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeXhtmlTransitional
  Create or change the doctype for a XHTML Transitional document
  --------------------------------------------------------------------------*/
void CreateDoctypeXhtmlTransitional (Document doc, View view)
{
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  CreateOrChangeDoctype (doc, view, L_Transitional, 0, TRUE, useMathML, useSVG);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeXhtmlStrict
  Create or change the doctype for a XHTML 1.0 Strict document
  --------------------------------------------------------------------------*/
void CreateDoctypeXhtmlStrict (Document doc, View view)
{
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  CreateOrChangeDoctype (doc, view, L_Strict, 0, TRUE, useMathML, useSVG);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeXhtmlBasic
  Create or change the doctype for a XHTML 1.0 Basic document
  --------------------------------------------------------------------------*/
void CreateDoctypeXhtmlBasic (Document doc, View view)
{
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  CreateOrChangeDoctype (doc, view, L_Basic, 0, TRUE, useMathML, useSVG);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeXhtmlRDFa
  Create or change the doctype for a XHTML+RDFa document
  --------------------------------------------------------------------------*/
void CreateDoctypeXhtmlRDFa (Document doc, View view)
{ 
  CreateOrChangeDoctype (doc, view, L_Xhtml11, L_RDFa, TRUE, 0, 0);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtmlTransitional
  Create or change the doctype for a HTML Transitional document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtmlTransitional (Document doc, View view)
{
  CreateOrChangeDoctype (doc, view, L_Transitional, 0, FALSE, FALSE, FALSE);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeHtmlStrict
  Create or change the doctype for a HTML Strict document
  --------------------------------------------------------------------------*/
void CreateDoctypeHtmlStrict (Document doc, View view)
{
  CreateOrChangeDoctype (doc, view, L_Strict, 0, FALSE, FALSE, FALSE);
  UpdateEditorMenus (doc);
}

/*--------------------------------------------------------------------------
  CreateDoctypeMathML
  Create or change the doctype for a MathML document
  --------------------------------------------------------------------------*/
void CreateDoctypeMathML (Document doc, View view)
{
  CreateOrChangeDoctype (doc, view, L_MathML, 0, TRUE, FALSE, FALSE);
}

/*--------------------------------------------------------------------------
  CreateDoctypeSVG
  Create or change the doctype for a SVG Basic document
  --------------------------------------------------------------------------*/
void CreateDoctypeSVG (Document doc, View view)
{
  ThotBool	  useMathML, useSVG;
 
  HasNatures (doc, &useMathML, &useSVG);
  CreateOrChangeDoctype (doc, view, L_SVG, 0, TRUE, useMathML, useSVG);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PasteBuffer (Document doc, View view)
{
#ifdef BOOKMARKS
  if (DocumentTypes[doc] == docBookmark)
    BM_PasteHandler (doc, view); /* bookmarks make their own cut and paste */
  else
#endif /* BOOKMARKS */
    TtcPaste (doc, view);
}

/*----------------------------------------------------------------------
  GiveNSDeclaration
  ----------------------------------------------------------------------*/
static int GiveNSDeclaration (Document document, Element el, char *buf)
{
#define MAX_NS 20
  char     *declarations[MAX_NS];
  char     *prefixes[MAX_NS];
  int       i, len;
  int       nb = 0;
  int       index = 0;

  // Fill up the element NS list
  for (i = 0; i < MAX_NS; i++)
    {
      declarations[i] = NULL;
      prefixes[i] = NULL;
    }
  
  TtaGiveElemNamespaceDeclarations (document, el, &declarations[0], &prefixes[0], MAX_NS);
  CurrentDocument = document;
  
  for (i = 0; i < MAX_NS; i++)
    {
      if (prefixes[i] == NULL && declarations[i] == NULL)
	i = MAX_NS;
      else
	{
	  if (declarations[i] != NULL)
	    {
	      nb++;
	      if (prefixes[i] != NULL)
		{
		  len = strlen(declarations[i]) + strlen(prefixes[i]) + 4; /* EOS + '=' + 2 x '"' */
		  strcpy (&buf[index], prefixes[i]);
		  index += strlen(prefixes[i]);
		  strcat (&buf[index++], "=");
		  strcat (&buf[index++], "\"");
		  strcat (&buf[index], declarations[i]);
		  index += strlen(declarations[i]);
		  strcat (&buf[index++], "\"");
		  buf[index++] = EOS;
		}
	      else
		{
		  len = strlen(declarations[i]) + 3; /* EOS + 2 x '"' */
		  strcpy (&buf[index++], "\"");
		  strcat (&buf[index], declarations[i]);
		  index += strlen(declarations[i]);
		  strcat (&buf[index++], "\"");
		  buf[index++] = EOS;
		}
	    }
	}
    }
  return nb;
}


/*----------------------------------------------------------------------
  UpdateNSDeclaration
  Update the Namespace declaration for a selected element
  ----------------------------------------------------------------------*/
void UpdateNSDeclaration (Document document, View view)
{
  Element   root = NULL;
  char      buf[MAX_TXT_LEN];
  int       nb = 0;
  //Element   el;
  //int       f, l;
  void     *p_dlg;

  /*
  TtaGiveFirstSelectedElement (document, &el, &f, &l);
  if (el != NULL)
    nb = GiveNSDeclaration (document, el, buf);
  */

  root = TtaGetRootElement (document);
  if (root)
    nb = GiveNSDeclaration (document, root, buf);

      // Create the dialog
      p_dlg = CreateListNSDlgWX (BaseDialog + ListNSForm,
				 TtaGetViewFrame (document, view),
				 nb, buf, RDFa_list );
      if (p_dlg)
	{
	  NSList = p_dlg;
	  TtaSetDialoguePosition ();
	  TtaShowDialogue (BaseDialog + ListNSForm, FALSE, TRUE);
	  /* wait for an answer */
	  TtaWaitShowDialogue ();
	}
}

/*----------------------------------------------------------------------
  CloseNSDeclaration
  ----------------------------------------------------------------------*/
void CloseNSDeclaration ()
{
  NSList = NULL;
}

/*----------------------------------------------------------------------
  SynchronizeNSDeclaration
  ----------------------------------------------------------------------*/
void SynchronizeNSDeclaration (NotifyElement *event)
{
  char      buf[MAX_TXT_LEN];
  int       nb = 0;
  wxArrayString wx_items;

  if (NSList)
    {
      if (((ListNSDlgWX *) NSList)->IsShown())
	{
	  nb = GiveNSDeclaration (event->document, event->element, buf);
	  /* Update the NS list */
	  UpdateListNSDlgWX (nb, buf, NSList);
	}
    }
}

/*----------------------------------------------------------------------
  Add a NS Declaration
  ----------------------------------------------------------------------*/
void AddaNSDeclaration (char *decl)
{
  Document  doc;
  Element   el = NULL;
  //int       f, l;
  char     *prefix = NULL, *url = NULL;

  doc = CurrentDocument;
  /*
  TtaGiveFirstSelectedElement (doc, &el, &f, &l);
  */
  el = TtaGetRootElement (doc);
  if (el)
    {
      url = strstr (decl, "=");
      if (url)
	{
	  url[0] = EOS;
	  prefix = decl;
	  decl = url + 1;
	}
      if (decl[0] == '\"')
	{
	  decl++;
	  url = strstr (decl, "\"");
	  if (url)
	    url[0] = EOS;
	}
      TtaSetANamespaceDeclaration (doc, el, prefix, decl);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  Remove a NS Declaration
  ----------------------------------------------------------------------*/
void RemoveaNSDeclaration (char *decl)
{
  Document  doc;
  Element   el = NULL;
  //int       f, l;
  char     *prefix = NULL, *url = NULL;

  doc = CurrentDocument;
  /*
  TtaGiveFirstSelectedElement (doc, &el, &f, &l);
  */
  el = TtaGetRootElement (doc);
  if (el)
    {
      url = strstr (decl, "=");
      if (url)
	{
	  url[0] = EOS;
	  prefix = decl;
	  decl = url + 1;
	}
      if (decl[0] == '\"')
	{
	  decl++;
	  url = strstr (decl, "\"");
	  if (url)
	    url[0] = EOS;
	}
      TtaRemoveANamespaceDeclaration (doc, el, prefix, decl);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SpellCheck (Document doc, View view)
{
  Element             docEl, el, body;
  ElementType         elType;
  int                 firstchar, lastchar;

  docEl = TtaGetMainRoot (doc);
  elType = TtaGetElementType (docEl);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    {
      elType.ElTypeNum = HTML_EL_BODY;
      body = TtaSearchTypedElement (elType, SearchInTree, docEl);
      if (body == NULL)
        return;
      /* get the current selection */
      TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
      if (el == NULL)
        {
          /* no current selection in the document */
          /* select the first character in the body */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          el = TtaSearchTypedElement (elType, SearchInTree, body);
          if (el != NULL)
            TtaSelectString (doc, el, 1, 0);
        }
    }
  TtcSpellCheck (doc, view);
}

/*----------------------------------------------------------------------
  CreateBreak
  ----------------------------------------------------------------------*/
void CreateBreak (Document doc, View view)
{
  ElementType         elType;
  Element             el, br, parent;
  DisplayMode         dispMode;
  int                 firstChar, lastChar;

  TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    return;
  elType = TtaGetElementType (el);
  if (TtaIsXmlSSchema (elType.ElSSchema))
    {      
      elType.ElTypeNum = XML_EL_xmlbr;
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      if (TtaCreateElement (elType, doc))
        {
          TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
          el = TtaGetParent (el);
          br = el;
          TtaNextSibling (&el);
          if (el == NULL)
            {
              el = br;
              TtaPreviousSibling (&el);
              firstChar = TtaGetElementVolume (el) + 1;
            }
          else
            firstChar = 1;
          if (el)
            {
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                TtaSelectString (doc, el, firstChar, firstChar - 1);
              else
                TtaSelectElement (doc, el);
            }
        }
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
  else if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      /* within HTML element */
      elType.ElTypeNum = HTML_EL_BR;
      if (TtaCreateElement (elType, doc))
        {
          TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
          br = el;
          TtaNextSibling (&el);
          if (el == NULL)
            {
              /* Insert a text element after the BR */
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              el = TtaNewElement (doc, elType);
              TtaInsertSibling (el, br, FALSE, doc);
              /* move the selection */
              TtaSelectString (doc, el, 1, 0);
            }
          else
            {
              /* move the selection */
              parent = el;
              while (el != NULL && !TtaIsLeaf (TtaGetElementType (el)))
                {
                  parent = el;
                  el = TtaGetFirstChild (parent);
                }
              if (el == NULL)
                TtaSelectElement (doc, parent);
              else
                {
                  elType = TtaGetElementType (el);
                  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                    TtaSelectString (doc, el, 1, 0);
                  else
                    TtaSelectString (doc, el, 0, 0);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  InsertWithinHead moves the insertion point into the document Head to
  insert the element type.
  Return TRUE if it succeeds.
  ----------------------------------------------------------------------*/
Element InsertWithinHead (Document doc, View view, int elementT)
{
  ElementType         elType;
  Element             el, firstSel, lastSel, head, parent, new_, title;
  SSchema             docSchema;
  int                 j, firstChar, lastChar, line;
  ThotBool            before;

  docSchema = TtaGetDocumentSSchema (doc);
  if (strcmp(TtaGetSSchemaName (docSchema), "HTML") != 0)
    /* not within an HTML document */
    return (NULL);
  else
    {
      elType.ElSSchema = docSchema;
      elType.ElTypeNum = HTML_EL_HEAD;
      el = TtaGetMainRoot (doc);
      head = TtaSearchTypedElement (elType, SearchForward, el);
      if (head && TtaIsReadOnly (head))
        return NULL;

      /* give current position */
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &j);
      TtaGiveLastSelectedElement (doc, &lastSel, &j, &lastChar);
      el = firstSel;
      if (firstSel == NULL || firstSel == head ||
          !TtaIsAncestor (firstSel, head))
        {
          /* the current selection is not within the head */
          el = TtaGetLastChild (head);
          /* insert after the last element in the head */
          before = FALSE;
        }
      else
        {
          /* the current selection is within the head */
          parent = TtaGetParent (el);
          /* does the selection precede the title? */
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == HTML_EL_TITLE)
            /* the title is selected, insert after it */
            before = FALSE;
          else
            {
              elType.ElTypeNum = HTML_EL_TITLE;
              title = TtaSearchTypedElement (elType, SearchForward, el);
              if (title != NULL)
                {
                  /* insert after the title */
                  before = FALSE;
                  el = title;
                }
              else
                {
                  /* insert before the current element */
                  before = TRUE;
                  while (parent != head)
                    {
                      el = parent;
                      /* insert after the parent element */
                      before = FALSE;
                      parent = TtaGetParent (el);
                    }
                }
            }
        }
      /* now insert the new element after el */
      elType.ElTypeNum = elementT;
      if (elementT == HTML_EL_BASE)
        /* the element to be created is BASE */
        {
          /* return if this element already exists */
          if (TtaSearchTypedElement (elType, SearchInTree, head))
            return (NULL);
        }
      new_ = TtaNewTree (doc, elType, "");
      // give the same line number as the current element
      line = TtaGetElementLineNumber (el);
      TtaSetElementLineNumber (new_, line);
      TtaInsertSibling (new_, el, before, doc);
      if (elementT != HTML_EL_SCRIPT_)
        {
          /* register this element in the editing history */
          TtaOpenUndoSequence (doc, firstSel, lastSel, firstChar,
                               lastChar);
          TtaRegisterElementCreate (new_, doc);
          TtaCloseUndoSequence (doc);
        }
      TtaSetDocumentModified (doc);
      return (new_);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateBase (Document doc, View view)
{
  Element             el;

  el = InsertWithinHead (doc, view, HTML_EL_BASE);
  if (el)
    TtaSelectElement (doc, el);
#ifdef _WX
  TtaRedirectFocus();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateMeta (Document doc, View view)
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  ThotBool            created;

  el = InsertWithinHead (doc, view, HTML_EL_META);
  if (el)
    {
      created = CreateMetaDlgWX (BaseDialog + TitleForm,
                                 TtaGetViewFrame (doc, view));
      if (created)
        {
          TtaSetDialoguePosition ();
          TtaShowDialogue (BaseDialog + TitleForm, FALSE, TRUE);
          /* wait for an answer */
          TtaWaitShowDialogue ();
        }
      if (MetaContent && (MetaEquiv || MetaName))
        {
          // add meta attributes
          TtaExtendUndoSequence (doc);
          // re-register the created element
          TtaCancelLastRegisteredOperation (doc);
          elType = TtaGetElementType (el);
          attrType.AttrSSchema = elType.ElSSchema;
          if (MetaName)
            attrType.AttrTypeNum = HTML_ATTR_meta_name;
          else
            attrType.AttrTypeNum = HTML_ATTR_http_equiv;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          if (MetaName)
            TtaSetAttributeText (attr, MetaName, el, doc);
          else
            TtaSetAttributeText (attr, MetaEquiv, el, doc);
          attrType.AttrTypeNum = HTML_ATTR_meta_content;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          TtaSetAttributeText (attr, MetaContent, el, doc);
          TtaRegisterElementCreate (el, doc);
          TtaCloseUndoSequence (doc);
          TtaSelectElement (doc, el);
        }
      else
        {
          // not a valid meta
          TtaDeleteTree (el, doc);
          TtaCancelLastRegisteredSequence (doc);
        }
      TtaFreeMemory (MetaContent);
      MetaContent = NULL;
      TtaFreeMemory (MetaEquiv);
      MetaEquiv = NULL;
      TtaFreeMemory (MetaName);
      MetaName = NULL;
    }
#ifdef _WX
  TtaRedirectFocus();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateLinkInHead (Document doc, View view)
{
  Element             el;

  el = InsertWithinHead (doc, view, HTML_EL_LINK); 
  if (el)
    {
      /* The link element is a new created one */
      UseLastTarget = FALSE;
      IsNewAnchor = TRUE;
      /* Select a new destination */
      SelectDestination (doc, el, FALSE, FALSE);
    }
#ifdef _WX
  //TtaRedirectFocus();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateStyle (Document doc, View view)
{
  Element             el, child;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;

  /* Don't check the Thot abstract tree against the structure schema. */
  TtaSetStructureChecking (FALSE, doc);
  el = InsertWithinHead (doc, view, HTML_EL_STYLE_);
  TtaSetStructureChecking (TRUE, doc);
  if (el)
    {
      /* create an attribute type="text/css" */
      TtaExtendUndoSequence (doc);
      // re-register the created element
      TtaCancelLastRegisteredOperation (doc);
      elType = TtaGetElementType (el);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_Notation;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, "text/css", el, doc);
      TtaRegisterElementCreate (el, doc);
      child = TtaGetFirstChild (el);
      if (child)
        TtaSelectElement (doc, child);
      else
        TtaSelectElement (doc, el);
      TtaCloseUndoSequence (doc);
    }
#ifdef _WX
  TtaRedirectFocus();
#endif /* _WX */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateComment (Document doc, View view)
{
  Element             el, parent = NULL;
  ElementType         elType;
  char               *s;
  int                 first, last;

  TtaGiveFirstSelectedElement (doc, &el, &first, &last);
  if (el)
    {
      /* get the structure schema of the parent element */
      parent = TtaGetParent (el);
      if (parent)
        el = parent;
      elType = TtaGetElementType (el);
    }
  else
    elType.ElSSchema = TtaGetDocumentSSchema (doc);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp (s, "MathML"))
    elType.ElTypeNum = MathML_EL_XMLcomment;
#ifdef _SVG
  else if (!strcmp (s, "SVG"))
    elType.ElTypeNum = SVG_EL_XMLcomment;
#endif /* _SVG */
  else
    elType.ElTypeNum = HTML_EL_Comment_;
  TtaInsertElement (elType, doc);
#ifdef _WX
  TtaRedirectFocus();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateDate (Document doc, View view)
{
  Element             el, parent = NULL, comment, child, text;
  ElementType         elType, elTypeText;
  char               *s;
  char                tm[500];
  int                 first, last;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  TtaGiveFirstSelectedElement (doc, &el, &first, &last);
  elType.ElTypeNum = 0;
  elType.ElSSchema = NULL;
  if (el == NULL)
    /* no selection */
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
  else if (TtaIsReadOnly (el))
    /* no selection */
    TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
  else
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      if (!strcmp (s, "HTML"))
        elType.ElTypeNum = HTML_EL_Comment_;
#ifdef _SVG
      else if (!strcmp (s, "SVG"))
        elType.ElTypeNum = SVG_EL_XMLcomment;
#endif /* _SVG */
#ifdef XML_GENERIC
      else if (!strcmp (s, "XML"))
        elType.ElTypeNum = XML_EL_xmlcomment;
#endif /* XML_GENERIC */
      else
        {
          /* look for an HTML parent element */
          parent = TtaGetParent (el);
          el = NULL;
          while (parent)
            {
              elType = TtaGetElementType (parent);
              if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                {
                  el = parent;
                  parent = NULL;
                  elType.ElTypeNum = HTML_EL_Comment_;
                }
              else
                parent = TtaGetParent (parent);
            }
        }
    }

  if (el)
    {
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      /* insert the text element */
      elTypeText.ElSSchema = elType.ElSSchema;
      elTypeText.ElTypeNum = HTML_EL_TEXT_UNIT;
      TtaInsertElement (elTypeText, doc);
      TtaGiveFirstSelectedElement (doc, &el, &first, &last);
      TtaGetTime (tm, UTF_8);
      TtaSetTextContent (el, (unsigned char*)tm, UTF_8, doc);
      TtaExtendUndoSequence (doc);

      /* insert the first comment */
      comment = TtaNewTree (doc, elType, "");
      TtaInsertSibling (comment, el, TRUE, doc);
      child = text = comment;
      while (child)
        {
          text = child;
          child = TtaGetFirstChild (text);
        }
      TtaSetTextContent (text, (unsigned char*)"$date=", UTF_8, doc);
      TtaRegisterElementCreate (comment, doc);
      text = TtaNewTree (doc, elTypeText, "");
      TtaInsertSibling (text, comment, TRUE, doc);
      TtaSetTextContent (text, (unsigned char*)" ", UTF_8, doc);
      TtaRegisterElementCreate (text, doc);

      /* insert the last comment */
      comment = TtaNewTree (doc, elType, "");
      TtaInsertSibling (comment, el, FALSE, doc);
      child = text = comment;
      while (child)
        {
          text = child;
          child = TtaGetFirstChild (text);
        }
      TtaSetTextContent (text, (unsigned char*)"$", UTF_8, doc);
      TtaRegisterElementCreate (comment, doc);
      text = TtaNewTree (doc, elTypeText, "");
      TtaInsertSibling (text, comment, FALSE, doc);
      TtaSetTextContent (text, (unsigned char*)" ", UTF_8, doc);
      TtaRegisterElementCreate (text, doc);
      TtaCloseUndoSequence (doc);
      TtaSelectElement (doc, el);
    }
}

/*----------------------------------------------------------------------
  CreateScript
  ----------------------------------------------------------------------*/
void CreateScript (Document doc, View view, ThotBool externalFile)
{
  SSchema             docSchema;
  ElementType         elType, headType;
  Element             el, child, parent = NULL;
  char               *s;
  int                 i, j;
  ThotBool            generateCDATA;

  /* test if we have to insert a script in the document head */
  TtaGiveFirstSelectedElement (doc, &el, &i, &j);
  docSchema = TtaGetDocumentSSchema (doc);
  if (el)
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      if (strcmp (s, "HTML") || elType.ElTypeNum != HTML_EL_HEAD)
        {
          headType.ElSSchema = elType.ElSSchema;
          headType.ElTypeNum = HTML_EL_HEAD;
          parent = TtaGetTypedAncestor (el, headType);
          if (parent)
            {
              el = parent;
              elType.ElTypeNum = HTML_EL_HEAD;
            }
          else
            {
              /* within a head 
                 get the structure schema of the parent element */
              parent = TtaGetParent (el);
              if (parent)
                el = parent;
              elType = TtaGetElementType (el);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
        }
    }
  else
    {
      elType.ElSSchema = docSchema;
      elType.ElTypeNum = 0;
      s = NULL;
    }

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  if (s == NULL || strcmp (s, "HTML") || elType.ElTypeNum == HTML_EL_HEAD)
    {
      /* cannot insert at the current position */
      if (!strcmp (TtaGetSSchemaName (docSchema), "HTML"))
        /* insert within the head of this HTML document */
        el = InsertWithinHead (doc, view, HTML_EL_SCRIPT_);
      else
        el = NULL;
    }
  else
    {
      /* create Script in the body if we are within an HTML document
         and within an HTML element */
      elType.ElTypeNum = HTML_EL_SCRIPT_;
      if (externalFile)
        {
        /* insert a script element after the selected element */
        TtaInsertSibling (TtaNewElement (doc, elType), el, FALSE, doc);
        TtaNextSibling(&el);
        }
      else
        {
        /* Insert a script element and open the structure view */
        TtaInsertElement (elType, doc);
        TtaGiveFirstSelectedElement (doc, &el, &i, &j);
        }
    }

  if (el)
    {
      if (externalFile)
        {
        /* register new created elements */
        TtaRegisterElementCreate (el, doc);
        LinkAsJavascript = TRUE;
        SelectDestination (doc, el, FALSE, FALSE);
        }
      else
        {
        TtaGetEnvBoolean ("GENERATE_CDATA", &generateCDATA);
        if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat &&
            generateCDATA)
          /* insert a cdata within */
          elType.ElTypeNum = HTML_EL_CDATA;
        else
          /* insert a comment within */
          elType.ElTypeNum = HTML_EL_Comment_;

        child = TtaNewTree (doc, elType, "");
        elType = TtaGetElementType (el);
        if (TtaIsLeaf (elType))
          {
            TtaInsertSibling (child, el, TRUE, doc);
            TtaDeleteTree (el, doc);
          }
        else
          TtaInsertFirstChild (&child, el, doc);

        /* register new created elements */
        TtaRegisterElementCreate (el, doc);
        while (child)
          {
            el = child;
            child = TtaGetFirstChild (el);
          }
        if (child)
          TtaSelectElement (doc, child);
        else
          TtaSelectElement (doc, el);
        }
    }
  TtaCloseUndoSequence (doc);
#ifdef _WX
  TtaRedirectFocus();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  HTMLelementAllowed
  ----------------------------------------------------------------------*/
ThotBool HTMLelementAllowed (Document doc)
{
  ElementType         elType;
  Element             el, ancestor, sibling;
  char               *s;
  int                 firstChar, lastChar;
  ThotBool            within_use = FALSE;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return FALSE;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      /* no selection */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return FALSE;
    }
  
  elType = TtaGetElementType (el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  ancestor = el;
  while (ancestor && !strcmp (s, "Template"))
    {
      if (elType.ElTypeNum == Template_EL_component)
        {
          // no structure check within a template component
          if (!within_use)
            TtaSetStructureChecking (FALSE, doc);
          ancestor = NULL;
        }
      else
        {
          if (!within_use)
            within_use = (elType.ElTypeNum == Template_EL_component);
          ancestor = TtaGetParent (ancestor);
          if (ancestor)
            {
              elType = TtaGetElementType (ancestor);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
        }
    }

  if (strcmp (s, "HTML") == 0)
    /* within an HTML element */
    return TRUE;
#ifdef TEMPLATES
  if (strcmp (s, "Template") == 0)
    /* within a template */
    return TRUE;
#endif /* TEMPLATES */
#ifdef _SVG
  else if (strcmp (s, "SVG") == 0)
    {
      CreateGraphicElement (doc, 2, 9);
      return TRUE;
    }
#endif /* _SVG */
  else
    /* not within an HTML element */
    {
      /* if the selection is at the beginning or at the end of an equation
         or a drawing, TtaCreateElement will create the new HTML element right
         before or after the MathML or Graphics object. */
      if (!TtaIsSelectionEmpty())
        return FALSE;
      
      if (firstChar <= 1)
        /* selection starts at the beginning of an element */
        {
          sibling = el;
          ancestor = el;
          TtaPreviousSibling (&sibling);
          while (sibling == NULL && ancestor != NULL)
            {
              ancestor = TtaGetParent (ancestor);
              if (ancestor)
                {
                  elType = TtaGetElementType (ancestor);
                  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                    /* this is an HTML element */
                    return TRUE;
                  sibling = ancestor;
                  TtaPreviousSibling (&sibling);
                }
            }
        }
     
      if ((lastChar == 0 && firstChar == 0) ||
          lastChar >=  TtaGetElementVolume (el))
        /* selection is at the end of an element */
        {
          sibling = el;
          ancestor = el;
          TtaNextSibling (&sibling);
          while (sibling == NULL && ancestor != NULL)
            {
              ancestor = TtaGetParent (ancestor);
              if (ancestor)
                {
                  elType = TtaGetElementType (ancestor);
                  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                    /* this is an HTML element */
                    return TRUE;
                  sibling = ancestor;
                  TtaNextSibling (&sibling);
                }
            }
        }
      return FALSE;
    }
}

/*----------------------------------------------------------------------
  CreateHTMLelement
  ----------------------------------------------------------------------*/
ThotBool CreateHTMLelement (int typeNum, Document doc)
{
  DisplayMode         dispMode;
  ElementType         elType;
  ThotBool            done, oldStructureChecking;

  oldStructureChecking = TtaGetStructureChecking (doc);
  if (HTMLelementAllowed (doc))
    {
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, SuspendDisplay);
      elType.ElSSchema = TtaGetSSchema ("HTML", doc);
      elType.ElTypeNum = typeNum;
      done = TtaCreateElement (elType, doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
  else
    done = FALSE;
  TtaSetStructureChecking (oldStructureChecking, doc);
  return done;
}

/*----------------------------------------------------------------------
  CreateParagraph
  ----------------------------------------------------------------------*/
void CreateParagraph (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Paragraph, doc);
}

/*----------------------------------------------------------------------
  CreateHeading1
  ----------------------------------------------------------------------*/
void CreateHeading1 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H1, doc);
}

/*----------------------------------------------------------------------
  CreateHeading2
  ----------------------------------------------------------------------*/
void CreateHeading2 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H2, doc);
}

/*----------------------------------------------------------------------
  CreateHeading3
  ----------------------------------------------------------------------*/
void CreateHeading3 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H3, doc);
}

/*----------------------------------------------------------------------
  CreateHeading4
  ----------------------------------------------------------------------*/
void CreateHeading4 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H4, doc);
}

/*----------------------------------------------------------------------
  CreateHeading5
  ----------------------------------------------------------------------*/
void CreateHeading5 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H5, doc);
}

/*----------------------------------------------------------------------
  CreateHeading6
  ----------------------------------------------------------------------*/
void CreateHeading6 (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_H6, doc);
}

/*----------------------------------------------------------------------
  CreateAreaMap
  ----------------------------------------------------------------------*/
void CreateMap (Document doc, View view)
{
  ElementType    elType;
  Element        el, div, map, p;
  int            i, j;
  ThotBool       oldStructureChecking;

  if (CreateHTMLelement (HTML_EL_Division, doc))
    {
      TtaExtendUndoSequence (doc);
      TtaGiveFirstSelectedElement (doc, &el, &i, &j);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_map;
      div = TtaGetParent (el);
      map = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&map, div, doc);
      TtaDeleteTree (el, doc);
      // generate the id and or name attribute
      CreateTargetAnchor (doc, map, FALSE, FALSE, TRUE);
      // generate a division
      elType.ElTypeNum = HTML_EL_Division;
      div = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&div, map, doc);
      elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
      p =  TtaNewElement (doc, elType);
      TtaInsertFirstChild (&p, div, doc);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      el =  TtaNewElement (doc, elType);
      TtaInsertFirstChild (&el, p, doc);
      TtaRegisterElementCreate (map, doc);
      TtaSelectElement (doc, el);
      TtaSetStructureChecking (oldStructureChecking, doc);
      // it should include a link
      CreateOrChangeLink (doc, view);
    }
}

/*----------------------------------------------------------------------
  IndentListItem
  ----------------------------------------------------------------------*/
void IndentListItem (Document doc, View view)
{
  DisplayMode         dispMode;
  ElementType         elType;
  Element             el, last, item, child, sibling;
  SSchema             sshtml;
  int                 firstChar, lastChar, i;

  sshtml  = TtaGetSSchema ("HTML", doc);
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el && sshtml)
    {
      TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);
      // look for the enclosing list item
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == sshtml &&
          (elType.ElTypeNum == HTML_EL_Unnumbered_List ||
           elType.ElTypeNum == HTML_EL_Numbered_List))
        return;
      if (elType.ElSSchema != sshtml ||
          elType.ElTypeNum != HTML_EL_List_Item)
        {
          elType.ElSSchema = sshtml;
          elType.ElTypeNum = HTML_EL_List_Item;
          item = TtaGetExactTypedAncestor (el, elType);
        }
      else
        item = el;
      if (item == NULL)
        return;

      elType = TtaGetElementType (last);
      if (last == el)
        last = item;
      else if (elType.ElSSchema != sshtml ||
               elType.ElTypeNum != HTML_EL_List_Item)
        {
          elType.ElSSchema = sshtml;
          elType.ElTypeNum = HTML_EL_List_Item;
          last = TtaGetExactTypedAncestor (last, elType);
          if (last == NULL)
            return;
        }
      // locate a previous list item
      sibling = item;
      TtaPreviousSibling (&sibling);
      if (sibling == NULL)
        return;
      // get the last children of the previous item
      child = TtaGetFirstChild (sibling);
      while (child)
        {
          sibling = child;
          TtaNextSibling (&child);
        }

      /* stop displaying changes that will be made */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      // move selected list items into a new sub-list
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      // create the sub-list
      elType.ElTypeNum = HTML_EL_Unnumbered_List;
      elType.ElSSchema = sshtml;
      el = TtaNewElement (doc, elType);
      TtaInsertSibling (el, sibling, FALSE, doc);
      child = item;
      sibling = NULL;
      do
        {
          item = child;
          TtaNextSibling (&child);
          TtaRegisterElementDelete (item, doc);
          TtaRemoveTree (item, doc);
          if (sibling)
            TtaInsertSibling (item, sibling, FALSE, doc);
          else
            TtaInsertFirstChild (&item, el, doc);
          sibling = item;
        }
      while (item != last);
      TtaRegisterElementCreate (el, doc);
      TtaCloseUndoSequence (doc);
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
      TtaSelectElement (doc, el);
    }
}

/*----------------------------------------------------------------------
  IndentListItem
  ----------------------------------------------------------------------*/
void UnindentListItem (Document doc, View view)
{
  DisplayMode         dispMode;
  ElementType         elType;
  Element             el, last, item, child, sibling, list, sublist;
  SSchema             sshtml;
  int                 firstChar, lastChar, i;
  ThotBool            emptylist = FALSE;

  sshtml  = TtaGetSSchema ("HTML", doc);
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el && sshtml)
    {
      TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);
      // look for the enclosing list item
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == sshtml &&
          (elType.ElTypeNum == HTML_EL_Unnumbered_List ||
           elType.ElTypeNum == HTML_EL_Numbered_List))
          {
            list = el;
            emptylist = TRUE;
            item = TtaGetFirstChild (el);
            child = item;
            while (child)
              {
                last = child;
                TtaNextSibling (&child);
              }
          }
      else
        {
          if (elType.ElSSchema != sshtml ||
              elType.ElTypeNum != HTML_EL_List_Item)
            {
              elType.ElSSchema = sshtml;
              elType.ElTypeNum = HTML_EL_List_Item;
              item = TtaGetExactTypedAncestor (el, elType);
            }
          else
            item = el;
          if (item == NULL)
            return;

          elType = TtaGetElementType (last);
          if (last == el)
            last = item;
          else if (elType.ElSSchema != sshtml ||
                   elType.ElTypeNum != HTML_EL_List_Item)
            {
              elType.ElSSchema = sshtml;
              elType.ElTypeNum = HTML_EL_List_Item;
              last = TtaGetExactTypedAncestor (last, elType);
              if (last == NULL)
                return;
            }
          // locate a previous list item in the enclosing list
          list = TtaGetParent (item);
          child = item;
          TtaPreviousSibling (&child);
          emptylist = FALSE;
          if (child == NULL)
            {
              child = last;
              TtaNextSibling (&child);
              if (child == NULL)
                // the whole list will be removed
                emptylist = TRUE;
            }
        }

      elType.ElSSchema = sshtml;
      elType.ElTypeNum = HTML_EL_List_Item;
      sibling = TtaGetExactTypedAncestor (list, elType);
      if (sibling == NULL)
        // cannot unindent
        return;

      /* stop displaying changes that will be made */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      child = item;
      el = NULL;
      do
        {
          item = child;
          TtaNextSibling (&child);
          TtaRegisterElementDelete (item, doc);
          TtaRemoveTree (item, doc);
          TtaInsertSibling (item, sibling, FALSE, doc);
          TtaRegisterElementCreate (item, doc);
          sibling = item;
          if (el == NULL)
            // register the first created item
            el = item;
        }
      while (item != last);
      if (emptylist)
        {
          // delete the sub-list
          TtaRegisterElementDelete (list, doc);
          TtaDeleteTree (list, doc);
        }
      else if (child)
        {
          // create a new empty sublist with same attributes
          sublist = TtaCopyTree (list, doc, doc, last);
          sibling = TtaGetFirstChild (sublist);
          do
            {
              item = sibling;
              TtaNextSibling (&sibling);              
              TtaRemoveTree (item, doc);
            }
          while (sibling);
          item = TtaGetFirstChild (last);
          while (item)
            {
              sibling = item;
              TtaNextSibling (&item);
            }
          TtaInsertSibling (sublist, sibling, FALSE, doc);

          // move also the end of the current sub-list
          sibling = NULL;
          do
            {
              item = child;
              TtaNextSibling (&child);
              TtaRegisterElementDelete (item, doc);
              TtaRemoveTree (item, doc);
              if (sibling)
                TtaInsertSibling (item, sibling, FALSE, doc);
              else
                TtaInsertFirstChild (&item, sublist, doc);
              sibling = item;
            }
          while (item);
        }
      TtaCloseUndoSequence (doc);
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
      TtaSelectElement (doc, el);
      if (last != el)
        TtaExtendSelection (doc, last, 0);
    }
}

/*----------------------------------------------------------------------
  ReplaceEmptyItem checks if the created element is within an empty
  list item.
  Return true if the creation is already done or ignored
  ----------------------------------------------------------------------*/
ThotBool ReplaceEmptyItem (int typeNum, Document doc)
{
  ElementType         elType;
  Element             el, prev, child;
  char               *s;
  int                 firstChar, lastChar;
  ThotBool            result = FALSE, empty;

  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el)
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      empty = TtaGetElementVolume (el) == 0;
      while (empty &&
             (elType.ElTypeNum != HTML_EL_List_Item || strcmp (s, "HTML")))
        {
          // check if it's an empty content of an empty list item
          child = el;
          empty = FALSE;
          TtaPreviousSibling (&child);
          if (child == NULL)
            {
              child = el;
              TtaNextSibling (&child);
              if (child == NULL)
                {
                  el = TtaGetParent (el);
                  elType = TtaGetElementType (el);
                  s = TtaGetSSchemaName (elType.ElSSchema);
                   empty = TRUE;
                }
            }
        }
      if (empty &&
          elType.ElTypeNum == HTML_EL_List_Item && !strcmp (s, "HTML"))
        {
          prev = el;
          TtaPreviousSibling (&prev);
          if (prev)
            {
              // remove the empty list item
              TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
              TtaRegisterElementDelete (el, doc);
              TtaDeleteTree (el, doc);
              child = TtaGetFirstChild (prev);
              while (child)
                {
                  prev = child;
                  TtaNextSibling (&child);
                }
              // create the sub-list
              elType.ElTypeNum = typeNum;
              child = TtaNewTree (doc, elType, "");
              TtaInsertSibling (child, prev, FALSE, doc);
              TtaRegisterElementCreate (child, doc);
              TtaCloseUndoSequence (doc);
              // select the new created element
              while (child)
                {
                  el = child;
                  child = TtaGetFirstChild (el);
                }
              if (child)
                TtaSelectElement (doc, child);
              else
                TtaSelectElement (doc, el);
            }
          result = TRUE;          
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  CreateList
  ----------------------------------------------------------------------*/
void CreateList (Document doc, View view)
{
  if (!ReplaceEmptyItem (HTML_EL_Unnumbered_List, doc))
    CreateHTMLelement (HTML_EL_Unnumbered_List, doc);
}

/*----------------------------------------------------------------------
  CreateNumberedList
  ----------------------------------------------------------------------*/
void CreateNumberedList (Document doc, View view)
{
  if (!ReplaceEmptyItem (HTML_EL_Numbered_List, doc))
    CreateHTMLelement (HTML_EL_Numbered_List, doc);
}

/*----------------------------------------------------------------------
  CreateDefinitionList
  ----------------------------------------------------------------------*/
void CreateDefinitionList (Document doc, View view)
{
  if (!ReplaceEmptyItem (HTML_EL_Definition_List, doc))
    CreateHTMLelement (HTML_EL_Definition_List, doc);
}

/*----------------------------------------------------------------------
  CreateDefinitionTerm
  ----------------------------------------------------------------------*/
void CreateDefinitionTerm (Document doc, View view)
{
  ElementType   elType;
  Element       dd, el, set, parent, sibling, list, child;
  int           firstChar, lastChar;

  if (TtaIsSelectionUnique ())
    {
      // check if a <dd> is selected
      TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_Definition &&
          !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
      {
        // get the enclosing Term_List
        parent = TtaGetParent(el);
        // check if a Definitions element follows the Term_List
        list = parent;
        TtaPreviousSibling (&list);
        if (list)
          {
            elType = TtaGetElementType (list);
            if (elType.ElTypeNum != HTML_EL_Term_List ||
                strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
              list = NULL;
          }

        // check if there is a previous definition
        sibling = el;
        TtaPreviousSibling (&sibling);
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
        if (sibling)
          {
            // create a new Definition_Item set
            sibling = el;
            TtaNextSibling (&sibling);
            parent = TtaGetParent (parent);
            elType.ElTypeNum = HTML_EL_Definition_Item;
            set = TtaNewElement (doc, elType);
            TtaInsertSibling (set, parent, FALSE, doc);
            // create a new Term list
            elType.ElTypeNum = HTML_EL_Term_List;
            list = TtaNewElement (doc, elType);
            TtaInsertFirstChild  (&list, set, doc);
            if (sibling)
              {
                // create a new Definition list after
                elType.ElTypeNum = HTML_EL_Definitions;
                parent = TtaNewElement (doc, elType);
                TtaInsertSibling (parent, list, FALSE, doc);
                // move all next terms
                child = NULL;
                while (sibling)
                  {
                    dd = sibling;
                    TtaNextSibling (&sibling);
                    TtaRegisterElementDelete (dd, doc);
                    TtaRemoveTree (dd, doc);
                    if (child)
                      TtaInsertSibling (dd, child, TRUE, doc);
                    else
                      TtaInsertFirstChild  (&dd, parent, doc);
                    child = dd;
                  }
              }
            TtaRegisterElementCreate (set, doc);
          }

        // transform a <dd> into a <dt>
        parent = TtaGetParent(el);
        set = TtaGetParent(parent);
        TtaRegisterElementDelete (el, doc);
        TtaRemoveTree (el, doc);
        TtaChangeTypeOfElement (el, doc, HTML_EL_Term);
        if (TtaGetFirstChild (parent) == NULL)
          {
            // remove empty term list
            TtaRegisterElementDelete (parent, doc);
            TtaDeleteTree (parent, doc);
            parent = NULL;
          }
        if (list == NULL)
          {
            // create a new Definitions list
            elType.ElTypeNum = HTML_EL_Term_List;
            list = TtaNewElement (doc, elType);
            if (parent)
              TtaInsertSibling (list, parent, TRUE, doc);
            else
              TtaInsertFirstChild  (&list, set, doc);
            // insert the term there
            TtaInsertFirstChild  (&el, list, doc);
            TtaRegisterElementCreate (list, doc);
          }
        else
          {
            child = TtaGetLastChild (list);
            if (child)
              TtaInsertSibling (el, child, FALSE, doc);
            else
              TtaInsertFirstChild  (&el, list, doc);
            TtaRegisterElementCreate (el, doc);
          }

        TtaSelectElement (doc, el);
        TtaCloseUndoSequence (doc);
        return;
      }      
    }
  CreateHTMLelement (HTML_EL_Term, doc);
}

/*----------------------------------------------------------------------
  CreateDefinitionDef
  ----------------------------------------------------------------------*/
void CreateDefinitionDef (Document doc, View view)
{
  ElementType   elType;
  Element       dt, el, set, parent, sibling, list, child;
  int           firstChar, lastChar;

  if (TtaIsSelectionUnique ())
    {
      // check if a <dt> is selected
      TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_Term &&
          !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
      {
        // get the enclosing Term_List
        parent = TtaGetParent(el);
        // check if a Definitions element follows the Term_List
        list = parent;
        TtaNextSibling (&list);
        if (list)
          {
            elType = TtaGetElementType (list);
            if (elType.ElTypeNum != HTML_EL_Definitions ||
                strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
              list = NULL;
          }
        // check if there is a next term
        sibling = el;
        TtaNextSibling (&sibling);
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
        if (sibling)
          {
            // create a new Definition_Item set
            parent = TtaGetParent (parent);
            elType.ElTypeNum = HTML_EL_Definition_Item;
            set = TtaNewElement (doc, elType);
            TtaInsertSibling (set, parent, FALSE, doc);
            // create a new Term list
            elType.ElTypeNum = HTML_EL_Term_List;
            parent = TtaNewElement (doc, elType);
            TtaInsertFirstChild  (&parent, set, doc);
            if (list)
              {
                // move the following Definitions list
                TtaRegisterElementDelete (list, doc);
                TtaRemoveTree (list, doc);
                TtaInsertSibling (list, parent, FALSE, doc);
                TtaRegisterElementCreate (list, doc);
                list = NULL;
              }
            // move all next terms
            child = NULL;
            while (sibling)
              {
                dt = sibling;
                TtaNextSibling (&sibling);
                TtaRegisterElementDelete (dt, doc);
                TtaRemoveTree (dt, doc);
                if (child)
                  TtaInsertSibling (dt, child, TRUE, doc);
                else
                  TtaInsertFirstChild  (&dt, parent, doc);
                child = dt;
              }
            TtaRegisterElementCreate (set, doc);
          }

        // transform a <dd> into a <dt>
        parent = TtaGetParent(el);
        set = TtaGetParent(parent);
        TtaRegisterElementDelete (el, doc);
        TtaRemoveTree (el, doc);
        TtaChangeTypeOfElement (el, doc, HTML_EL_Definition);
        if (TtaGetFirstChild (parent) == NULL)
          {
            // remove empty term list
            TtaRegisterElementDelete (parent, doc);
            TtaDeleteTree (parent, doc);
            parent = NULL;
          }
        if (list == NULL)
          {
            // create a new Definitions list
            elType.ElTypeNum = HTML_EL_Definitions;
            list = TtaNewElement (doc, elType);
            if (parent)
              TtaInsertSibling (list, parent, FALSE, doc);
            else
              TtaInsertFirstChild  (&list, set, doc);
            // insert the definition there
            TtaInsertFirstChild  (&el, list, doc);
            TtaRegisterElementCreate (list, doc);
          }
        else
          {
            child = TtaGetFirstChild (list);
            if (child)
              TtaInsertSibling (el, child, TRUE, doc);
            else
              TtaInsertFirstChild  (&el, list, doc);
            TtaRegisterElementCreate (el, doc);
          }

        TtaSelectElement (doc, el);
        TtaCloseUndoSequence (doc);
        return;
      }      
    }
  CreateHTMLelement (HTML_EL_Definition, doc);
}

/*----------------------------------------------------------------------
  CreateHorizontalRule
  ----------------------------------------------------------------------*/
void CreateHorizontalRule (Document doc, View view)
{
  if (CreateHTMLelement (HTML_EL_Horizontal_Rule, doc))
    {
      // then insert an empty element after
      TtaExtendUndoSequence (doc);  
      InsertAfter (doc, view);
      TtaCloseUndoSequence (doc);
    }
}

/*----------------------------------------------------------------------
  CreateBlockQuote
  ----------------------------------------------------------------------*/
void CreateBlockQuote (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Block_Quote, doc);
}

/*----------------------------------------------------------------------
  CreatePreformatted
  ----------------------------------------------------------------------*/
void CreatePreformatted (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Preformatted, doc);
}

/*----------------------------------------------------------------------
  CreateRuby
  Create a ruby element.
  If the current selection is simply a caret, create an empty simple ruby at
  that position.
  If some text/elements are selected, create a simple ruby element at that
  position and move the selected elements within the rb element.
  ----------------------------------------------------------------------*/
void CreateRuby (Document doc, View view)
{
  ElementType   elType;
  Element       el, selEl, rbEl, rubyEl, firstEl, lastEl, nextEl, prevEl;
  int           i, j, lg, firstSelectedChar, lastSelectedChar, min, max;
  Language      lang;
  CHAR_T        *buffer;
  ThotBool      error;
  DisplayMode   dispMode;
  ThotBool      oldStructureChecking;

  elType.ElSSchema = TtaGetSSchema ("HTML", doc);
  elType.ElTypeNum = HTML_EL_complex_ruby;
  /* if we are already within a ruby element, return immediately */
  TtaGiveFirstSelectedElement (doc, &firstEl, &firstSelectedChar, &j);
  if (TtaGetTypedAncestor (firstEl, elType))
    return;
  elType.ElTypeNum = HTML_EL_simple_ruby;
  if (TtaGetTypedAncestor (firstEl, elType))
    return;

  oldStructureChecking = TtaGetStructureChecking (doc);
  if (HTMLelementAllowed (doc))
    {
      /* stop displaying changes that will be made */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      selEl = NULL;
      rbEl = NULL;
      rubyEl = NULL;
      if (TtaIsSelectionEmpty())
        /* selection is simply a caret */
        {
          /* create a simple_ruby element at the current position */
          TtaCreateElement (elType, doc);
          /* get the rb element that has just been created within the ruby */
          TtaGiveFirstSelectedElement (doc, &el, &i, &j);
          if (el)
            {
              selEl = el; /* empty leaf within the rb element */
              rbEl = TtaGetParent (el);
              /* get the CHOICE element that follows the rb element and
                 delete it */
              el = rbEl;
              TtaNextSibling (&el);
              if (el)
                TtaDeleteTree (el, doc);
              rubyEl = TtaGetParent (rbEl);
            }
        }
      else if (IsCharacterLevelElement (firstEl))
        /* there are some elements/text selected. Make it the content
           of the rb element */
        {
          TtaGiveLastSelectedElement(doc, &lastEl, &i, &lastSelectedChar);
          if (TtaGetParent (firstEl) == TtaGetParent (lastEl))
            /* all selected elements are siblings */
            {
              /* check if there are some ruby elements within the selected
                 elements */
              error = FALSE;
              el = firstEl;
              while (el && !error)
                {
                  elType = TtaGetElementType (el);
                  if ((elType.ElTypeNum == HTML_EL_simple_ruby ||
                       elType.ElTypeNum == HTML_EL_complex_ruby) &&
                      !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                    /* it's a ruby element. Error */
                    error = TRUE;
                  else
                    {
                      elType.ElSSchema = TtaGetSSchema ("HTML", doc);
                      elType.ElTypeNum = HTML_EL_simple_ruby;
                      if (TtaSearchTypedElement (elType, SearchInTree, el))
                        error = TRUE;
                      else
                        {
                          elType.ElTypeNum = HTML_EL_complex_ruby;
                          if (TtaSearchTypedElement (elType, SearchInTree, el))
                            error = TRUE;
                        }
                    }
                  TtaGiveNextElement (doc, &el, lastEl);
                }
              if (!error)
                {
                  TtaUnselect (doc);
                  /* split the last element if it's a character string */
                  elType = TtaGetElementType (lastEl);
                  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                    /* it's a text element */
                    {
                      lg = TtaGetElementVolume (lastEl);
                      if (lastSelectedChar <= lg && lastSelectedChar > 1)
                        /* the last selected element is only partly selected.
                           Split it */
                        {
                          /* exclude trailing spaces from the selection */
                          if (lg > 0)
                            {
                              lg++;
                              buffer = (CHAR_T*)TtaGetMemory (lg * sizeof(CHAR_T));
                              TtaGiveBufferContent (lastEl, buffer, lg, &lang);
                              if (lastEl == firstEl)
                                min = firstSelectedChar;
                              else
                                min = 1;
                              while (lastSelectedChar > min &&
                                     buffer[lastSelectedChar - 2] == SPACE)
                                lastSelectedChar--;
                              TtaFreeMemory (buffer);
                            }
                          TtaSplitText (lastEl, lastSelectedChar, doc);
                        }
                    }
                  /* process the first selected element */
                  elType = TtaGetElementType (firstEl);
                  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
                    /* it's a text element */
                    if (firstSelectedChar > 1)
                      /* that element is only partly selected. Split it */
                      {
                        el = firstEl;
                        lg = TtaGetElementVolume (firstEl);
                        /* exclude leading spaces from the selection */
                        if (lg > 0)
                          {
                            lg++;
                            buffer = (CHAR_T*)TtaGetMemory (lg * sizeof(CHAR_T));
                            TtaGiveBufferContent (firstEl, buffer, lg, &lang);
                            if (lastEl == firstEl)
                              max = lastSelectedChar;
                            else
                              max = lg;
                            while (firstSelectedChar < max &&
                                   buffer[firstSelectedChar - 1] == SPACE)
                              firstSelectedChar++;
                            TtaFreeMemory (buffer);
                          }
                        if (firstSelectedChar <= lg)
                          {
                            TtaSplitText (firstEl, firstSelectedChar,doc);
                            TtaNextSibling (&firstEl);
                          }
                        if (lastEl == el)
                          /* we have to change the end of selection because the
                             last selected element was split */
                          lastEl = firstEl;
                      }
                  /* create a ruby element with a rb element and moves all
                     selected elements within the new rb element */
                  TtaOpenUndoSequence (doc, firstEl, lastEl, 0, 0);
                  elType.ElTypeNum = HTML_EL_simple_ruby;
                  rubyEl = TtaNewElement (doc, elType);
                  TtaInsertSibling (rubyEl, firstEl, TRUE, doc);
                  elType.ElTypeNum = HTML_EL_rb;
                  rbEl = TtaNewElement (doc, elType);
                  TtaInsertFirstChild (&rbEl, rubyEl, doc);
                  TtaRegisterElementCreate (rubyEl, doc);
                  el = firstEl;
                  prevEl = NULL;
                  while (el)
                    {
                      if (el == lastEl)
                        nextEl = NULL;
                      else
                        {
                          nextEl = el;
                          TtaGiveNextElement (doc, &nextEl, lastEl);
                        }
                      TtaRegisterElementDelete (el, doc);
                      TtaRemoveTree (el, doc);
                      if (!prevEl)
                        TtaInsertFirstChild  (&el, rbEl, doc);
                      else
                        TtaInsertSibling (el, prevEl, FALSE, doc);
                      prevEl = el;
                      TtaRegisterElementCreate (el, doc);
                      el = nextEl;
                    }
                  TtaCloseUndoSequence (doc);
                }
            }
        }
      if (rbEl)
        /* a rb element has been created. create the other elements within
           the new ruby element */
        {
          /* create a first rp element after the rb element */
          elType.ElTypeNum = HTML_EL_rp;
          el = TtaNewTree (doc, elType, "");
          TtaInsertSibling (el, rbEl, FALSE, doc);
          prevEl = el;
          el = TtaGetFirstChild (el);
          TtaSetTextContent (el, (unsigned char*)"(", TtaGetDefaultLanguage (), doc);
          /* create a rt element after the first rp element */
          elType.ElTypeNum = HTML_EL_rt;
          el = TtaNewTree (doc, elType, "");
          TtaInsertSibling (el, prevEl, FALSE, doc);
          if (!selEl)
            /* nothing to be selected. Select the first leaf within the
               new rt element */
            selEl = TtaGetFirstChild (el);
          prevEl = el;
          /* create a second rp element after the rt element */
          elType.ElTypeNum = HTML_EL_rp;
          el = TtaNewTree (doc, elType, "");
          TtaInsertSibling (el, prevEl, FALSE, doc);
          el = TtaGetFirstChild (el);
          TtaSetTextContent (el, (unsigned char*)")", TtaGetDefaultLanguage (), doc);
          /* create a text element after the ruby element, to allow the
             user to add some more text after the ruby */
          el = rubyEl;
          TtaNextSibling (&el);
          if (!el)
            {
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              el = TtaNewElement (doc, elType);
              TtaInsertSibling (el, rubyEl, FALSE, doc);
            }
        }
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
      /* update the selection */
      if (selEl)
        TtaSelectElement (doc, selEl);
    }
  TtaSetStructureChecking (oldStructureChecking, doc);
}

/*----------------------------------------------------------------------
  CreateAddress
  ----------------------------------------------------------------------*/
void CreateAddress (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Address, doc);
}

/*----------------------------------------------------------------------
  DoTableCreation 
  ----------------------------------------------------------------------*/
void DoTableCreation (Document doc)
{
  ElementType         elType;
  Element             el, new_, caption, cell, row, colstruct, cols, prevCol,
                      child;
  AttributeType       attrType;
  Attribute           attr;
  int                 firstChar, i, profile;
  char                stylebuff[100];
  ThotBool            loadcss;

  /* get the new Table element */
  TtaSetDisplayMode (doc, SuspendDisplay);
  TtaLockTableFormatting ();
  elType.ElSSchema = TtaGetSSchema ("HTML", doc);
  elType.ElTypeNum = HTML_EL_Table_;
  TtaCreateElement (elType, doc);
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &i);
  if (el)
    {
      profile = TtaGetDocumentProfile(doc);
      if (TCaption == 1)
        {
          // insert the caption
          elType.ElTypeNum = HTML_EL_CAPTION;
          caption = TtaNewTree (doc, elType, "");
          TtaInsertFirstChild (&caption, el, doc);
        }
      else
        caption = NULL;

      /* create a COL element for each column */
      if (NumberCols > 0)
        {
          elType.ElTypeNum = HTML_EL_ColStruct;
          colstruct = TtaNewElement (doc, elType);
          if (caption)
            TtaInsertSibling (colstruct, caption, FALSE, doc);
          else
            TtaInsertFirstChild (&colstruct, el, doc);
          elType.ElTypeNum = HTML_EL_Cols;
          cols = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&cols, colstruct, doc);
          elType.ElTypeNum = HTML_EL_COL;
          prevCol = NULL;
          while (NumberCols > 0)
            {
              new_ = TtaNewTree (doc, elType, "");
              if (prevCol)
                TtaInsertSibling (new_, prevCol, FALSE, doc);
              else
                TtaInsertFirstChild (&new_, cols, doc);
              prevCol = new_;
              NumberCols--;
            }
        }

      /* manage the border attribute */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_Border;
      attr = TtaGetAttribute (el, attrType);
      if (profile == L_Basic)
        {
          if (attr)
            /* remove the Border attribute */
            TtaRemoveAttribute (el, attr, doc);
        }
      else
        {
          if (attr == NULL)
            /* the Table has no Border attribute, create one */
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          TtaSetAttributeValue (attr, TBorder, el, doc);
        }

      /* generate width style */
      if (TMAX_Width || profile == L_Basic)
        {
          attrType.AttrTypeNum = HTML_ATTR_Style_;
          attr = TtaNewAttribute (attrType);
          if (profile == L_Basic)
            {
              if (TMAX_Width)
                sprintf (stylebuff, "width: 100%%; border: solid %dpx", TBorder);
              else
                sprintf (stylebuff, "border: solid %dpx", TBorder);
            }
          else if (TMAX_Width)
            strcpy (stylebuff, "width: 100%");
          else
            stylebuff[0] = EOS;
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeText (attr, stylebuff, el, doc);	 
          /* check if we have to load CSS */
          TtaGetEnvBoolean ("LOAD_CSS", &loadcss);
          if (loadcss)
            ParseHTMLSpecificStyle (el, stylebuff, doc, 1000, FALSE);
        }

      elType.ElTypeNum = HTML_EL_Table_cell;
      cell = TtaSearchTypedElement (elType, SearchInTree, el);
      elType.ElTypeNum = HTML_EL_Data_cell;
      if (cell == NULL)
        /* look for a data cell */
        cell = TtaSearchTypedElement (elType, SearchInTree, el);
      else
        /* replace the cell element by a data cell */
        TtaChangeTypeOfElement (cell, doc, HTML_EL_Data_cell);
      if (cell)
        {
          child = TtaGetFirstLeaf (cell);
          if (child == cell)
            /* create an empty Element as a child of the first cell */
            {
              elType.ElTypeNum = HTML_EL_Element;
              child = TtaNewElement (doc, elType);
              if (child)
                TtaInsertFirstChild (&child, cell, doc);
            }
          TtaSelectElement (doc, child);
        }
      if (NumberRows > 1)
        {
          elType.ElTypeNum = HTML_EL_Table_row;
          row = TtaSearchTypedElement (elType, SearchInTree, el);
          while (NumberRows > 1)
            {
              new_ = TtaNewElement (doc, elType);
              TtaInsertSibling (new_, row, FALSE, doc);
              NumberRows--;
            }
        } 
      CheckAllRows (el, doc, FALSE, FALSE);
    }

  /* close the undo sequence if it's still open */
  TtaCloseUndoSequence (doc);
  TtaUnlockTableFormatting ();
  TtaSetDisplayMode (doc, DisplayImmediately);
  UpdateContextSensitiveMenus (doc, 1);
}

/*----------------------------------------------------------------------
  CreateTable
  ----------------------------------------------------------------------*/
void CreateTable (Document doc, View view)
{
  ElementType         elType, elTypeFirst;
  Element             firstSel;
  SSchema             sch;
  int                 firstChar, lastChar;
  char               *name;
  ThotBool            withinTable, inMath, created, oldStructureChecking;

  withinTable = FALSE;
  inMath = FALSE;
  sch = TtaGetSSchema ("HTML", doc);
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (firstSel)
    {
      /* look for an enclosing cell */
      elTypeFirst = TtaGetElementType (firstSel);
      elType.ElSSchema = elTypeFirst.ElSSchema;
      name = TtaGetSSchemaName (elType.ElSSchema);
      if ((!strcmp (name, "MathML") && elTypeFirst.ElTypeNum == MathML_EL_MTABLE) ||
          (!strcmp (name, "HTML") && elTypeFirst.ElTypeNum == HTML_EL_Table_))
        return;
      else if (!strcmp (name, "MathML"))
        /* the current selection starts with a MathML element */
        {
          elType.ElTypeNum = MathML_EL_MTABLE;
          withinTable = (TtaGetExactTypedAncestor (firstSel, elType) != NULL);
          inMath = TRUE;
          if (withinTable &&
              (TtaIsColumnSelected (doc) ||
	       elTypeFirst.ElTypeNum == MathML_EL_MLABELEDTR ||
               elTypeFirst.ElTypeNum == MathML_EL_MTR ||
               elTypeFirst.ElTypeNum == MathML_EL_MTD))
            return;
        }
      else if (!strcmp (name, "HTML"))
        /* the current selection starts with a HTML element */
        {
          elType.ElTypeNum = HTML_EL_Table_;
          withinTable = (TtaGetExactTypedAncestor (firstSel, elType) != NULL);
          if (withinTable &&
              (TtaIsColumnSelected (doc) ||
	       elTypeFirst.ElTypeNum == HTML_EL_Table_row ||
               elTypeFirst.ElTypeNum == HTML_EL_Data_cell ||
               elTypeFirst.ElTypeNum == HTML_EL_Heading_cell))
            return;
        }
    }

  oldStructureChecking = TtaGetStructureChecking (doc);
  if (HTMLelementAllowed (doc))
    {
      elType.ElSSchema = sch;
      if (elType.ElSSchema)
        {
          /* check the selection */
          if (TtaIsSelectionEmpty ())
            /* selection empty.  Display the Table dialogue box */
            {
              TtaGetEnvInt ("TABLE_ROWS", &NumberRows);
              TtaGetEnvInt ("TABLE_COLUMNS", &NumberCols);
              TtaGetEnvInt ("TABLE_BORDER", &TBorder);
              created = CreateCreateTableDlgWX (BaseDialog + TableForm,
                                                TtaGetViewFrame (doc, view),
                                                NumberCols, NumberRows, TBorder);
              if (created)
                {
                  TtaShowDialogue (BaseDialog + TableForm, FALSE, TRUE);
                  /* wait for an answer */
                  TtaWaitShowDialogue ();
                }
              if (!UserAnswer)
                {
                  TtaSetStructureChecking (oldStructureChecking, doc);
                return;
                }
            }
          else
            TBorder = 1;
          /* create the table or 
             try to transform the current selection if the selection is not empty */
          DoTableCreation (doc);
        }
    }
  TtaSetStructureChecking (oldStructureChecking, doc);
}

/*----------------------------------------------------------------------
  SingleCreateTable
  Function used in Lite profiles
  ----------------------------------------------------------------------*/
void SingleCreateTable (Document doc, View view)
{
  CreateTable (doc, view);
}

/*----------------------------------------------------------------------
  DoCreateTable
  Function attached to the table button
  ----------------------------------------------------------------------*/
void DoCreateTable (Document doc, View view)
{
  CreateTable (doc, view);
}

/*----------------------------------------------------------------------
  CreateCaption
  If the selection is a position anywhere within a table, create an empty
  caption element in this table and set the selection in this new caption.
  If the caption element already exists, just select it.
  ----------------------------------------------------------------------*/
void CreateCaption (Document doc, View view)
{
  ElementType         elType;
  Element             el, caption;
  int                 i, j;

  TtaGiveFirstSelectedElement (doc, &el, &i, &j);
  if (el)
    /* there is a current selection */
    {
      elType = TtaGetElementType (el);
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* the selected element is an HTML element */
        {
          caption = NULL;
          /* is the selection within a table */
          if (elType.ElTypeNum != HTML_EL_Table_)
            {
              /* get the enclosing table element */
              elType.ElTypeNum = HTML_EL_Table_;
              el = TtaGetTypedAncestor (el, elType);
              if (el == NULL)
                /* we are not in a table. Stop */
                return;
            }
          elType.ElTypeNum = HTML_EL_CAPTION;
          caption =  TtaSearchTypedElement (elType, SearchInTree, el);
          if (!caption)
            {
              /* no caption yet. Select the first child of the table
                 to create a caption element there */
              el = TtaGetFirstChild (el);
              TtaSelectElement (doc, el);
              /* no caption yet. Create one */
              TtaCreateElement (elType, doc);
            }
        }
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}


/*----------------------------------------------------------------------
  FirstDescendantOfTypes
  Return the first descendant of element el that is of type (ss, type1)
  or (ss, type2)
  ----------------------------------------------------------------------*/
static Element FirstDescendantOfTypes (Element el, SSchema ss, int type1,
                                       int type2)
{
  Element             desc, result;
  ElementType         elType;

  result = NULL;
  desc = TtaGetFirstChild (el);
  while (!result && desc)
    {
      elType = TtaGetElementType (desc);
      if (elType.ElSSchema == ss &&
          (elType.ElTypeNum == type1 || elType.ElTypeNum == type2))
        result = desc;
      else
        {
          result = FirstDescendantOfTypes (desc, ss, type1, type2);
          while (!result && desc)
            {
              TtaNextSibling (&desc);
              result = FirstDescendantOfTypes (desc, ss, type1, type2);
            }
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  LastDescendantOfTypes
  Return the last descendant of element el that is of type (ss, type1)
  or (ss, type2)
  ----------------------------------------------------------------------*/
static Element LastDescendantOfTypes (Element el, SSchema ss, int type1,
                                       int type2)
{
  Element             desc, result;
  ElementType         elType;

  result = NULL;
  desc = TtaGetLastChild (el);
  while (!result && desc)
    {
      elType = TtaGetElementType (desc);
      if (elType.ElSSchema == ss &&
          (elType.ElTypeNum == type1 || elType.ElTypeNum == type2))
        result = desc;
      else
        {
          result = LastDescendantOfTypes (desc, ss, type1, type2);
          while (!result && desc)
            {
              TtaPreviousSibling (&desc);
              result = LastDescendantOfTypes (desc, ss, type1, type2);
            }
        }
    }
  return result;
}

/*----------------------------------------------------------------------
  ChangeOneCell
  if element el is a cell of type other, change it to typeCell
  ----------------------------------------------------------------------*/
static void ChangeOneCell (Document doc, Element el, SSchema HTMLSSchema,
                           int other, int typeCell, ThotBool *open)
{
  ElementType         elType;

  elType = TtaGetElementType (el);
  if (elType.ElSSchema == HTMLSSchema && elType.ElTypeNum == other)
    {
      /* change the type of this cell */
      TtaChangeTypeOfElement (el, doc, typeCell);
      /* open the undo sequence if it is not already open */
      if (!*open)
        {
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          *open = TRUE;
        }
      /* register the change in the undo sequence */
      TtaRegisterElementTypeChange (el, other, doc);
    }
}

/*----------------------------------------------------------------------
  ChangeCell
  transform all cells in the current selection into typeCell (which is
  either th or td)
  ----------------------------------------------------------------------*/
static void ChangeCell (Document doc, View view, int typeCell)
{
  Element             el, last, firstSel, lastSel, ancestor, cell, row, body;
  ElementType         elType;
  SSchema	            HTMLSSchema;
  int                 firstchar, lastchar, other;
  ThotBool            open = FALSE;

  if (typeCell == HTML_EL_Heading_cell)
    other = HTML_EL_Data_cell;
  else
    other = HTML_EL_Heading_cell;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstchar, &lastchar);
  if (firstSel != NULL)
    {
      if (TtaIsReadOnly (firstSel))
        /* the selected element is read-only */
        return;

      TtaGiveLastSelectedElement (doc, &lastSel, &firstchar, &lastchar);
      HTMLSSchema = TtaGetSSchema ("HTML", doc);
      el = firstSel;
      elType = TtaGetElementType (el);

      if (elType.ElSSchema != HTMLSSchema || 
          (elType.ElTypeNum != typeCell && elType.ElTypeNum != other))
        /* the first selected element is not a table cell */
        {
          /* check if there is a cell among its ancestors */
          elType.ElSSchema = HTMLSSchema;
          elType.ElTypeNum = other;
          ancestor = TtaGetTypedAncestor (el, elType);
          if (!ancestor)
            {
              elType.ElTypeNum = typeCell;
              ancestor = TtaGetTypedAncestor (el, elType);
            }
          if (ancestor)
            /* a cell ancestor was found. Start from that element */
            el = ancestor;
          else
            /* no cell ancestor */
            {
              elType = TtaGetElementType (el);
              if (elType.ElSSchema == HTMLSSchema &&
                  (elType.ElTypeNum == HTML_EL_Table_ ||
                   elType.ElTypeNum == HTML_EL_thead ||
                   elType.ElTypeNum == HTML_EL_tbody ||
                   elType.ElTypeNum == HTML_EL_tfoot))
                /* we are in an ascendant of a cell. Get the first descendant
                   cell */
                el = FirstDescendantOfTypes (el, HTMLSSchema, typeCell, other);
            } 
        }
      last = lastSel;
      elType = TtaGetElementType (last);
      if (elType.ElSSchema != HTMLSSchema || 
          (elType.ElTypeNum != typeCell && elType.ElTypeNum != other))
        /* the last selected element is not a table cell */
        {
          /* check if there is a cell among its ancestors */
          elType.ElSSchema = HTMLSSchema;
          elType.ElTypeNum = other;
          ancestor = TtaGetTypedAncestor (last, elType);
          if (!ancestor)
            {
              elType.ElTypeNum = typeCell;
              ancestor = TtaGetTypedAncestor (last, elType);
            }
          if (ancestor)
            /* a cell ancestor was found. Start from that element */
            last = ancestor;
          else
            /* no cell ancestor */
            {
              elType = TtaGetElementType (last);
              if (elType.ElSSchema == HTMLSSchema &&
                  (elType.ElTypeNum == HTML_EL_Table_ ||
                   elType.ElTypeNum == HTML_EL_thead ||
                   elType.ElTypeNum == HTML_EL_tbody ||
                   elType.ElTypeNum == HTML_EL_tfoot))
                /* we are in an ascendant of a cell. Get the last descendant
                   cell */
                last = LastDescendantOfTypes (last, HTMLSSchema, typeCell,
                                              other);
            } 
        }

      while (el)
        {
          elType = TtaGetElementType (el);
          if (elType.ElSSchema == HTMLSSchema && elType.ElTypeNum == other)
            /* it's a cell, change its type */
            ChangeOneCell (doc, el, HTMLSSchema, other, typeCell, &open);
          else if (elType.ElSSchema == HTMLSSchema && 
                   elType.ElTypeNum == HTML_EL_Table_row)
            /* it's a row, change the type of all included cells */
            {
              cell = TtaGetFirstChild (el);
              while (cell)
                {
                  ChangeOneCell (doc, cell, HTMLSSchema, other, typeCell,
                                 &open);
                  TtaNextSibling (&cell);
                }
            }
          else if (elType.ElSSchema == HTMLSSchema && 
                   (elType.ElTypeNum == HTML_EL_thead ||
                    elType.ElTypeNum == HTML_EL_tbody ||
                    elType.ElTypeNum == HTML_EL_tfoot))
            /* it's a thead, tbody or tfoot, change the type of all included
               cells */
            {
              row = TtaGetFirstChild (el);
              while (row)
                {
                  cell = TtaGetFirstChild (row);
                  while (cell)
                    {
                      ChangeOneCell (doc, cell, HTMLSSchema, other, typeCell,
                                     &open);
                      TtaNextSibling (&cell);
                    }
                  TtaNextSibling (&row);
                }
            }
          else if (elType.ElSSchema == HTMLSSchema && 
                   elType.ElTypeNum == HTML_EL_Table_body)
            /* it's a Table_body, change the type of all included cells */
            {
              body = TtaGetFirstChild (el);
              while (body)
                {
                  row = TtaGetFirstChild (body);
                  while (row)
                    {
                      cell = TtaGetFirstChild (row);
                      while (cell)
                        {
                          ChangeOneCell (doc, cell, HTMLSSchema, other, typeCell,
                                         &open);
                          TtaNextSibling (&cell);
                        }
                      TtaNextSibling (&row);
                    }
                  TtaNextSibling (&body);
                }
            }

          if (el == last)
            el = NULL;
          else
            TtaGiveNextElement (doc, &el, last);
        }
      if (open)
        TtaCloseUndoSequence (doc);
      TtaSelectElement (doc, firstSel);
      if (firstSel != lastSel)
        TtaExtendSelection (doc, lastSel, 0);
      TtaSetStatusSelectedElement(doc, view, firstSel);
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}

/*----------------------------------------------------------------------
  ChangeToDataCell
  ----------------------------------------------------------------------*/
void ChangeToDataCell (Document doc, View view)
{
  ChangeCell (doc, view, HTML_EL_Data_cell);
}

/*----------------------------------------------------------------------
  ChangeToHeadingCell
  ----------------------------------------------------------------------*/
void ChangeToHeadingCell (Document doc, View view)
{
  ChangeCell (doc, view, HTML_EL_Heading_cell);
}

/*----------------------------------------------------------------------
  GetEnclosingCell
  Return the table cell element that contains the beginning (if first)
  or the end of the current selection in document doc.
  If such an element exists, an undo sequence is opened when parameter
  openUndoSeq is TRUE.
  ----------------------------------------------------------------------*/
static Element GetEnclosingCell (Document doc, ThotBool first,
                                 ThotBool openUndoSeq)
{
  Element             el, firstSel, lastSel;
  ElementType         elType;
  char               *s;
  int                 firstchar, lastchar, i;

  el = NULL; firstSel = NULL; lastSel = NULL;
  /* get the first selected element */
  if (first)
    {
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstchar, &i);
      el = firstSel;
    }
  else
    {
      TtaGiveLastSelectedElement (doc, &lastSel, &lastchar, &i);
      el = lastSel;
    }
  if (el)
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      while (el &&
             (strcmp (s, "MathML") ||
              elType.ElTypeNum != MathML_EL_MTD) &&
             (strcmp (s, "HTML") ||
              (elType.ElTypeNum != HTML_EL_Data_cell &&
               elType.ElTypeNum != HTML_EL_Heading_cell)))
        {
          el = TtaGetParent (el);
          if (el)
            {
              elType = TtaGetElementType (el);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
        }
      if (el)
        {
          if (first)
            TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastchar);
          else
            TtaGiveFirstSelectedElement (doc, &lastSel, &i, &lastchar);
          if (openUndoSeq)
            TtaOpenUndoSequence (doc, firstSel, lastSel, firstchar, lastchar);
        }
    }
  return el;
}

/*----------------------------------------------------------------------
  CellVertExtend
  Merge the cell that contains the selection with the next cell in the
  same column.
  ----------------------------------------------------------------------*/
void CellVertExtend (Document doc, View view)
{
  Element       cell, nextCell, colhead, row;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           span, nextSpan, newSpan, i;
  ThotBool      inMath;
  DisplayMode   dispMode;

  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      elType = TtaGetElementType (cell);
      inMath = !TtaSameSSchemas (elType.ElSSchema, 
                                 TtaGetSSchema ("HTML", doc));
      /* get the column of the cell */
      attrType.AttrSSchema = elType.ElSSchema;
      if (inMath)
        attrType.AttrTypeNum = MathML_ATTR_MRef_column;
      else
        attrType.AttrTypeNum = HTML_ATTR_Ref_column;
      attr = TtaGetAttribute (cell, attrType);
      nextCell = NULL;
      if (attr)
        /* the cell has an attribute Ref_column */
        {
          TtaGiveReferenceAttributeValue (attr, &colhead);
          if (colhead)
            {
              /* get the rowspan of the cell */
              if (inMath)
                attrType.AttrTypeNum = MathML_ATTR_rowspan_;
              else
                attrType.AttrTypeNum = HTML_ATTR_rowspan_;
              attr = TtaGetAttribute (cell, attrType);
              if (!attr)
                span = 1;
              else
                {
                  span = TtaGetAttributeValue (attr);
                  if (span < 0)
                    span = 1;
                }
              /* if spanning is 0 (infinite), stop */
              if (span != 0)
                {
                  /* get the next row after the cell */
                  row = TtaGetParent (cell);
                  for (i = 1; i <= span && row; i++)
                    row = GetSiblingRow (row, FALSE, inMath);
                  if (row)
                    /* check if there is a cell in that row, in the same
                       column. Another cell could span horizontally and
                       make the merger impossible */
                    nextCell = GetCellFromColumnHead (row, colhead, inMath);
                }
            }
        }
      
      if (nextCell)
        /* we can extend the cell */
        {
          if (inMath)
            attrType.AttrTypeNum = MathML_ATTR_rowspan_;
          else
            attrType.AttrTypeNum = HTML_ATTR_rowspan_;

          attr = TtaGetAttribute (nextCell, attrType);
          if (attr)
            {
              nextSpan = TtaGetAttributeValue (attr);
              if (nextSpan < 0)
                nextSpan = 1;
            }
          else
            nextSpan = 1;

          attr = TtaGetAttribute (cell, attrType);
          if (attr)
            {
              span = TtaGetAttributeValue (attr);
              if (span < 1)
                span = 1;
            }
          else
            span = 1;
          if (nextSpan == 0)
            /* "infinite" spanning */
            newSpan = 0;
          else
            newSpan = span + nextSpan;
          /* merge the following cell(s) with the current cell */
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          ChangeRowspan (cell, span, &newSpan, doc);
          /* set and register the new value of attribute rowspan */
          if (!attr)
            {
              if (newSpan != 1)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (cell, attr, doc);
                  TtaSetAttributeValue (attr, newSpan, cell, doc);
                  TtaRegisterAttributeCreate (attr, cell, doc);
                }
            }
          else
            {
              if (newSpan == 1)
                {
                  TtaRegisterAttributeDelete (attr, cell, doc);
                  TtaRemoveAttribute (cell, attr, doc);
                }
              else
                {
                  TtaRegisterAttributeReplace (attr, cell, doc);
                  TtaSetAttributeValue (attr, newSpan, cell, doc);
                }
            }
          SetRowExt (cell, newSpan, doc, inMath);
          TtaCloseUndoSequence (doc);
          TtaSetDocumentModified (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, dispMode);
        }
    }
}

/*----------------------------------------------------------------------
  CellHorizExtend
  Merge the cell that contains the selection with the next cell in the
  same row.
  ----------------------------------------------------------------------*/
void CellHorizExtend (Document doc, View view)
{
  Element       cell, row, nextCell, colhead;
  ElementType   elType;
  Attribute     attr, colspanAttr;
  AttributeType attrType, colspanType;
  int           span, nextSpan, newSpan, i;
  ThotBool      inMath;
  DisplayMode   dispMode;

  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      elType = TtaGetElementType (cell);
      inMath = !TtaSameSSchemas (elType.ElSSchema,
                                 TtaGetSSchema ("HTML", doc));
      /* get the column of the cell */
      attrType.AttrSSchema = elType.ElSSchema;
      colspanType.AttrSSchema = elType.ElSSchema;
      if (inMath)
        {
          colspanType.AttrTypeNum = MathML_ATTR_columnspan;
          attrType.AttrTypeNum = MathML_ATTR_MRef_column;
        }
      else
        {
          colspanType.AttrTypeNum = HTML_ATTR_colspan_;
          attrType.AttrTypeNum = HTML_ATTR_Ref_column;
        }
      /* get the colspan of the cell */
      colspanAttr = TtaGetAttribute (cell, colspanType);
      if (!colspanAttr)
        span = 1;
      else
        {
          span = TtaGetAttributeValue (colspanAttr);
          if (span < 0)
            span = 1;
        }
      /* get the next cell */
      attr = TtaGetAttribute (cell, attrType);
      nextCell = NULL;
      if (attr)
        /* the cell has an attribute Ref_column */
        {
          TtaGiveReferenceAttributeValue (attr, &colhead);
          if (colhead)
            {
              /* if spanning is 0 (infinite), stop */
              if (span != 0)
                {
                  /* get the next column after the cell */
                  for (i = 1; i <= span && colhead; i++)
                    TtaNextSibling (&colhead);
                  if (colhead)
                    /* check if there is a cell in that column, in the same
                       row. Another cell could span horizontally and make the
                       merger impossible */
                    {
                      row = TtaGetParent (cell);
                      nextCell = GetCellFromColumnHead (row, colhead, inMath);
                    }
                }
            }
        }

      if (nextCell)
        /* we can extend the cell */
        {
          attr = TtaGetAttribute (nextCell, colspanType);
          if (attr)
            {
              nextSpan = TtaGetAttributeValue (attr);
              if (nextSpan < 0)
                nextSpan = 1;
            }
          else
            nextSpan = 1;

          if (nextSpan == 0)
            /* "infinite" spanning */
            newSpan = 0;
          else
            newSpan = span + nextSpan;
          /* merge the following cell(s) with the current cell */
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          ChangeColspan (cell, span, &newSpan, doc);
          SetColExt (cell, newSpan, doc, inMath, FALSE);
          /* we set and register the new value of attribute colspan after
             cells have actually merged. That way, when undoing the command,
             merged cells will be recreated and linked to their column with
             the right (old) value of the attribute */
          if (!colspanAttr)
            {
              if (newSpan != 1)
                {
                  colspanAttr = TtaNewAttribute (colspanType);
                  TtaAttachAttribute (cell, colspanAttr, doc);
                  TtaSetAttributeValue (colspanAttr, newSpan, cell, doc);
                  TtaRegisterAttributeCreate (colspanAttr, cell, doc);
                }
            }
          else
            {
              if (newSpan == 1)
                {
                  TtaRegisterAttributeDelete (colspanAttr, cell, doc);
                  TtaRemoveAttribute (cell, colspanAttr, doc);
                }
              else
                {
                  TtaRegisterAttributeReplace (colspanAttr, cell, doc);
                  TtaSetAttributeValue (colspanAttr, newSpan, cell, doc);
                }
            }
          TtaCloseUndoSequence (doc);
          TtaSetDocumentModified (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, dispMode);
        }
    }
}

/*----------------------------------------------------------------------
  GetEnclosingRow
  Return the table row element that contains the beginning of the
  current selection in document doc.
  ----------------------------------------------------------------------*/
static Element GetEnclosingRow (Document doc, ThotBool first)
{
  Element             el;
  ElementType         elType;
  char               *s;
  int                 firstchar, lastchar;

  /* get the first (or last) selected element */
  if (first)
    TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  else
    TtaGiveLastSelectedElement (doc, &el, &firstchar, &lastchar);
  if (el)
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      while (el &&
             (strcmp (s, "HTML") ||
              elType.ElTypeNum != HTML_EL_Table_row) &&
             (strcmp (s, "MathML") ||
              (elType.ElTypeNum != MathML_EL_MTR &&
               elType.ElTypeNum != MathML_EL_MLABELEDTR)))
        {
          el = TtaGetParent (el);
          if (el)
            {
              elType = TtaGetElementType (el);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
        }
    }
  return el;
}

/*----------------------------------------------------------------------
  CanMergeSelectedCells
  Check if command "Merge selected cells" can be applied to the
  current selection
  ----------------------------------------------------------------------*/
ThotBool CanMergeSelectedCells (Document doc)
{
  Element       cell, lastCell;
  ThotBool      ok;

  ok = FALSE;
  cell = GetEnclosingCell (doc, TRUE, FALSE);
  if (cell)
    /* the current selection starts with a cell */
    {
      lastCell = GetEnclosingCell (doc, FALSE, FALSE);
      if (lastCell && lastCell != cell)
        /* is end with another cell. OK. */
        ok = TRUE;
    }
  else
    {
      if (GetEnclosingRow (doc, TRUE) && GetEnclosingRow (doc, FALSE))
        ok = TRUE;
    }
  return ok;
}

/*----------------------------------------------------------------------
  MergeSelectedCells
  Merge theall selected cells into a single cell.
  ----------------------------------------------------------------------*/
void MergeSelectedCells (Document doc, View view)
{
  Element       firstCell, lastCell, cell, firstRow, lastRow, row, firstCol,
                lastCol, col;
  ElementType   elType;
  Attribute     attr, colspanAttr, rowspanAttr;
  AttributeType attrType;
  int           rowspan, colspan, span, nrow, ncol;
  ThotBool      inMath;
  DisplayMode   dispMode;

  /* get the cell containing the beginning of the current selection */
  lastCell = NULL;
  firstCell = GetEnclosingCell (doc, TRUE, FALSE);
  if (firstCell)
    {
      /* get the cell containing the end of the current selection */
      lastCell = GetEnclosingCell (doc, FALSE, FALSE);
      if (!lastCell || lastCell == firstCell)
        /* only one cell selected. Can't merge anything. */
        firstCell = NULL;
    }
  else
    {
      firstRow = GetEnclosingRow (doc, TRUE);
      lastRow = GetEnclosingRow (doc, FALSE);
      elType = TtaGetElementType (firstRow);
      inMath = !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML");  
      if (firstRow && lastRow)
        {
          firstCell = GetFirstCellOfRow (firstRow, inMath);
          cell = GetFirstCellOfRow (lastRow, inMath);
          while (cell)
            {
              lastCell = cell;
              cell = GetSiblingCell (cell, FALSE, inMath);
            }
        }
    }
  if (!firstCell || !lastCell)
    return;
  elType = TtaGetElementType (firstCell);
  inMath = !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML");  
  attrType.AttrSSchema = elType.ElSSchema;
  /* the current selection is supposed to contain only cells that have the
     shape of a rectangle */
  /* count the number of rows in the current selection */
  firstRow = TtaGetParent (firstCell);
  lastRow = TtaGetParent (lastCell);
  nrow = 0;
  row = firstRow;
  while (row)
    {
      nrow++;
      if (row == lastRow)
        row = NULL;
      else
        row = NextTableRow (row);
    }
  /* get the rowspan value of the last selected cell */
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_rowspan_;
  else
    attrType.AttrTypeNum = HTML_ATTR_rowspan_;
  rowspanAttr = TtaGetAttribute (firstCell, attrType);
  if (rowspanAttr)
    {
      rowspan = TtaGetAttributeValue (rowspanAttr);
      if (rowspan < 0)
        rowspan = 1;
    }
  else
    rowspan = 1;
  attr = TtaGetAttribute (lastCell, attrType);
  if (attr)
    {
      span = TtaGetAttributeValue (attr);
      if (span < 0)
        span = 1;
    }
  else
    span = 1;
  if (span == 0)
    /* "infinite" spanning */
    nrow = 0;
  else
    nrow = nrow + span - 1;

  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_columnspan;
  else
    attrType.AttrTypeNum = HTML_ATTR_colspan_;
  colspanAttr = TtaGetAttribute (firstCell, attrType);
  if (colspanAttr)
    {
      colspan = TtaGetAttributeValue (colspanAttr);
      if (colspan < 0)
        colspan = 1;
    }
  else
    colspan = 1;

  /* check all cells in the first row selected to find the rightmost
     column (lastCol) of the current selection */
  firstCol = TtaGetColumn (firstCell);
  lastCol = TtaGetColumn (lastCell); /* initial rightmost column in selection*/
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_MColExt;
  else
    attrType.AttrTypeNum = HTML_ATTR_ColExt;
  cell = firstCell;
  do
    {
      attr = TtaGetAttribute (cell, attrType);
      if (attr)
        /* this cell is extended horizontally. Get its rightmost col */
        TtaGiveReferenceAttributeValue (attr, &col);
      else
        col = TtaGetColumn (cell);
      if (TtaIsBefore (lastCol, col))
        /* this is the rightmost column we have seen so far */
        lastCol = col;
      if (cell == lastCell)
        cell = NULL;
      else
        {
          TtaGiveNextElement (doc, &cell, lastCell);
          if (cell && TtaGetParent (cell) != firstRow)
            /* end of first row */
            cell = NULL;
        }
    }
  while (cell);
  /* count the number of columns in the current selection */
  ncol = 0;
  col = firstCol;
  while (col)
    {
      ncol++;
      if (col == lastCol)
        col = NULL;
      else
        TtaNextSibling (&col);
    }
  /* merge cells */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  TtaOpenUndoSequence (doc, firstCell, lastCell, 0, 0);
  if (ncol != colspan)
    {
      ChangeColspan (firstCell, colspan, &ncol, doc);
      SetColExt (firstCell, ncol, doc, inMath, FALSE);
      if (!colspanAttr)
        {
          if (ncol != 1)
            {
              if (inMath)
                attrType.AttrTypeNum = MathML_ATTR_columnspan;
              else
                attrType.AttrTypeNum = HTML_ATTR_colspan_;
              colspanAttr = TtaNewAttribute (attrType);
              TtaAttachAttribute (firstCell, colspanAttr, doc);
              TtaSetAttributeValue (colspanAttr, ncol, firstCell, doc);
              TtaRegisterAttributeCreate (colspanAttr, firstCell, doc);
            }
        }
      else
        {
          if (ncol == 1)
            {
              TtaRegisterAttributeDelete (colspanAttr, firstCell, doc);
              TtaRemoveAttribute (firstCell, colspanAttr, doc);
            }
          else
            {
              TtaRegisterAttributeReplace (colspanAttr, firstCell, doc);
              TtaSetAttributeValue (colspanAttr, ncol, firstCell, doc);
            }
        }
    }
  if (nrow != rowspan)
    {
      ChangeRowspan (firstCell, rowspan, &nrow, doc);
      /* set and register the new value of attribute rowspan */
      if (!rowspanAttr)
        {
          if (nrow != 1)
            {
              if (inMath)
                attrType.AttrTypeNum = MathML_ATTR_rowspan_;
              else
                attrType.AttrTypeNum = HTML_ATTR_rowspan_;
              rowspanAttr = TtaNewAttribute (attrType);
              TtaAttachAttribute (firstCell, rowspanAttr, doc);
              TtaSetAttributeValue (rowspanAttr, nrow, firstCell, doc);
              TtaRegisterAttributeCreate (rowspanAttr, firstCell, doc);
            }
        }
      else
        {
          if (nrow == 1)
            {
              TtaRegisterAttributeDelete (rowspanAttr, firstCell, doc);
              TtaRemoveAttribute (firstCell, rowspanAttr, doc);
            }
          else
            {
              TtaRegisterAttributeReplace (rowspanAttr, firstCell, doc);
              TtaSetAttributeValue (rowspanAttr, nrow, firstCell, doc);
            }
        }
      SetRowExt (firstCell, nrow, doc, inMath);
    }
  TtaCloseUndoSequence (doc);
  TtaSetDocumentModified (doc);
  if (nrow != rowspan || ncol != colspan)
    TtaSelectElement (doc, firstCell);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  UpdateContextSensitiveMenus (doc, view);
}

/*----------------------------------------------------------------------
  CanVShrinkCell
  Check if command "Shrink cell vertically" can be applied to the
  current selection
  ----------------------------------------------------------------------*/
ThotBool CanVShrinkCell (Document doc)
{
  Element       cell, lastCell;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           span;
  ThotBool      inMath;

  cell = GetEnclosingCell (doc, TRUE, FALSE);
  if (!cell)
    return FALSE;
  lastCell = GetEnclosingCell (doc, FALSE, FALSE);
  if (!lastCell || lastCell != cell)
    return FALSE;
  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc));
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_rowspan_;
  else
    attrType.AttrTypeNum = HTML_ATTR_rowspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (!attr)
    return FALSE;
  span = TtaGetAttributeValue (attr);
  if (span == 0)
    span = GetActualColspan (cell, inMath);
  if (span < 2)
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  CellVertShrink
  ----------------------------------------------------------------------*/
void CellVertShrink (Document doc, View view)
{
  Element       cell, lastCell;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           span, newSpan;
  ThotBool      inMath;

  /* Function UpdateContextSensitiveMenus (HTMLactions.c) should deactivate
     the corresponding menu item (XHTML/Table/ShrinkVertical) if the
     current selection is not (within) a single table cell that has
     rowspan attribute. */
  cell = GetEnclosingCell (doc, TRUE, FALSE);
  if (!cell)
    return;
  lastCell = GetEnclosingCell (doc, FALSE, TRUE);
  if (!lastCell || lastCell != cell)
    return;
  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc));
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_rowspan_;
  else
    attrType.AttrTypeNum = HTML_ATTR_rowspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (attr)
    {
      span = TtaGetAttributeValue (attr);
      if (span == 0)
        span = GetActualRowspan (cell, inMath);
      if (span >= 2)
        {
          newSpan = span - 1;
          if (newSpan == 1)
            {
              TtaRegisterAttributeDelete (attr, cell, doc);
              TtaRemoveAttribute (cell, attr, doc);
            }
          else
            {
              TtaRegisterAttributeReplace (attr, cell, doc);
              TtaSetAttributeValue (attr, newSpan, cell, doc);
            }
          ChangeRowspan (cell, span, &newSpan, doc);
          SetRowExt (cell, newSpan, doc, inMath);
            TtaSetDocumentModified (doc);
        }
    }
  TtaCloseUndoSequence (doc);
  UpdateContextSensitiveMenus (doc, view);
}

/*----------------------------------------------------------------------
  CanHShrinkCell
  Check if command "Shrink cell horizontally" can be applied to the
  current selection
  ----------------------------------------------------------------------*/
ThotBool CanHShrinkCell (Document doc)
{
  Element       cell, lastCell;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           span;
  ThotBool      inMath;

  cell = GetEnclosingCell (doc, TRUE, FALSE);
  if (!cell)
    return FALSE;
  lastCell = GetEnclosingCell (doc, FALSE, FALSE);
  if (!lastCell || lastCell != cell)
    return FALSE;
  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc));
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_columnspan;
  else
    attrType.AttrTypeNum = HTML_ATTR_colspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (!attr)
    return FALSE;
  span = TtaGetAttributeValue (attr);
  if (span == 0)
    span = GetActualColspan (cell, inMath);
  if (span < 2)
    return FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  CellHorizShrink
  ----------------------------------------------------------------------*/
void CellHorizShrink (Document doc, View view)
{
  Element       cell, lastCell;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           span, newSpan;
  ThotBool      inMath;

  cell = GetEnclosingCell (doc, TRUE, FALSE);
  if (!cell)
    return;
  lastCell = GetEnclosingCell (doc, FALSE, TRUE);
  if (!lastCell || lastCell != cell)
    return;
  elType = TtaGetElementType (cell);
  inMath = !TtaSameSSchemas (elType.ElSSchema, TtaGetSSchema ("HTML", doc));
  attrType.AttrSSchema = elType.ElSSchema;
  if (inMath)
    attrType.AttrTypeNum = MathML_ATTR_columnspan;
  else
    attrType.AttrTypeNum = HTML_ATTR_colspan_;
  attr = TtaGetAttribute (cell, attrType);
  if (attr)
    {
      span = TtaGetAttributeValue (attr);
      if (span == 0)
        span = GetActualColspan (cell, inMath);
      if (span >= 2)
        {
          newSpan = span - 1;
          if (newSpan == 1)
            {
              TtaRegisterAttributeDelete (attr, cell, doc);
              TtaRemoveAttribute (cell, attr, doc);
            }
          else
            {
              TtaRegisterAttributeReplace (attr, cell, doc);
              TtaSetAttributeValue (attr, newSpan, cell, doc);
            }
          ChangeColspan (cell, span, &newSpan, doc);
          SetColExt (cell, newSpan, doc, inMath, FALSE);
          TtaSetDocumentModified (doc);
        }
    }
  TtaCloseUndoSequence (doc);
  UpdateContextSensitiveMenus (doc, view);

}

/*----------------------------------------------------------------------
  SelectRow
  Select the table row to which the first selected element belongs.
  ----------------------------------------------------------------------*/
void SelectRow (Document doc, View view)
{
  Element     el;

  el = GetEnclosingRow (doc, TRUE);
  if (el)
    {
      TtaSelectElement (doc, el);
      el = GetEnclosingRow (doc, FALSE);
      if (el)
        TtaExtendSelection (doc, el, 0);
    }
}

/*----------------------------------------------------------------------
  CreateRow
  ----------------------------------------------------------------------*/
static void CreateRow (Document doc, View view, ThotBool before)
{
  Element             el, elNew, cell, row, nextRow;
  Attribute           attr;
  ElementType         elType;
  AttributeType       attrType;
  int                 span, i;
  NotifyElement       event;
  DisplayMode         dispMode;
  ThotBool            inMath;

  row = NULL;
  if (!before)
    /* creating a row after the current selection */
    {
      /* if the end of the selection is (within) a cell that is extended
         vertically, find the row that corresponds to the bottom of that cell*/
      cell = GetEnclosingCell (doc, FALSE, TRUE);
      if (cell)
        {
          /* get the rowspan of the cell */
          elType = TtaGetElementType (cell);
          inMath = !TtaSameSSchemas (elType.ElSSchema,
                                     TtaGetSSchema ("HTML", doc));
          attrType.AttrSSchema = elType.ElSSchema;
          if (inMath)
            attrType.AttrTypeNum = MathML_ATTR_rowspan_;
          else
            attrType.AttrTypeNum = HTML_ATTR_rowspan_;
          attr = TtaGetAttribute (cell, attrType);
          if (!attr)
            span = 1;
          else
            {
              span = TtaGetAttributeValue (attr);
              if (span < 0)
                span = 1;
              if (span == 0)
                /* infinite spanning */
                span = 10000;
            }
          if (span > 1)
            {
              /* get the next row after the cell */
              row = TtaGetParent (cell);
              nextRow = GetSiblingRow (row, FALSE, inMath);
              for (i = 1; i < span && nextRow; i++)
                {
                  row = nextRow;
                  nextRow = GetSiblingRow (row, FALSE, inMath);
                }
            }
        }
    }
  if (!row)
    row = GetEnclosingRow (doc, before);
  if (row)
    {
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      elType = TtaGetElementType (row);
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      elNew = TtaNewElement (doc, elType);
      TtaInsertSibling (elNew, row, before, doc);
      event.element = elNew;
      event.document = doc;
      RowCreated (&event);
      TtaRegisterElementCreate (elNew, doc);
      TtaCloseUndoSequence (doc);
      el = TtaGetFirstLeaf (elNew);
      TtaSelectElement (doc, el);
      TtaSetDisplayMode (doc, dispMode);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  CreateRowBefore
  ----------------------------------------------------------------------*/
void CreateRowBefore (Document doc, View view)
{
  CreateRow (doc, view, TRUE);
}

/*----------------------------------------------------------------------
  CreateRowAfter
  ----------------------------------------------------------------------*/
void CreateRowAfter (Document doc, View view)
{
  CreateRow (doc, view, FALSE);
}

/*----------------------------------------------------------------------
  SelectColumn
  Select the table column to which the first selected element belongs.
  ----------------------------------------------------------------------*/
void SelectColumn (Document doc, View view)
{
  Element             el;
  int                 firstchar, lastchar;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  TtaSelectEnclosingColumn (el);
}

/*----------------------------------------------------------------------
  CreateColumn
  ----------------------------------------------------------------------*/
static void CreateColumn (Document doc, View view, ThotBool before)
{
  Element             cell, lastCell, elNew, colhead, lastColhead;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  DisplayMode         dispMode;
  ThotBool            inMath;

  /* get the enclosing cell */
  cell = GetEnclosingCell (doc, before, TRUE);
  if (cell)
    {
      elType = TtaGetElementType (cell);
      inMath = !strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML");  
      colhead = NULL;
      if (!before)
        {
          attrType.AttrSSchema = elType.ElSSchema;
          if (inMath)
            attrType.AttrTypeNum = MathML_ATTR_MColExt;
          else
            attrType.AttrTypeNum = HTML_ATTR_ColExt;
          lastCell = cell; /* last selected cell (in tree order) */
          lastColhead = TtaGetColumn (lastCell); /* rightmost column */
          cell = GetEnclosingCell (doc, TRUE, FALSE); /* first selected cell */
          /* check all cells in the current selection to find the rightmost
             column (lastColhead) to which a selected cell is extended */
          do
            {
              attr = TtaGetAttribute (cell, attrType);
              if (attr)
                /* this cell is extended horizontally. Get its rightmost colhead */
                TtaGiveReferenceAttributeValue (attr, &colhead);
              else
                colhead = TtaGetColumn (cell);
              if (TtaIsBefore (lastColhead, colhead))
                lastColhead = colhead;
              if (cell == lastCell)
                cell = NULL;
              else
                TtaGiveNextElement (doc, &cell, lastCell);
            }
          while (cell);
          colhead = lastColhead;
        }
      if (!colhead)
        colhead = TtaGetColumn (cell);
      if (colhead)
        {
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          /* Create the column */
          elNew = NewColumnHead (colhead, before, FALSE, NULL, doc, inMath, TRUE, TRUE);
          TtaSetDisplayMode (doc, dispMode);
          TtaSetDocumentModified (doc);
        }
      TtaCloseUndoSequence (doc);
    }
}

/*----------------------------------------------------------------------
  CreateColumnBefore
  ----------------------------------------------------------------------*/
void CreateColumnBefore (Document doc, View view)
{
  CreateColumn (doc, view, TRUE);
}

/*----------------------------------------------------------------------
  CreateColumnAfter
  ----------------------------------------------------------------------*/
void CreateColumnAfter (Document doc, View view)
{
  CreateColumn (doc, view, FALSE);
}

/*----------------------------------------------------------------------
  PasteBefore
  ----------------------------------------------------------------------*/
void PasteBefore (Document doc, View view)
{
  Element             cell, el, child;
  ElementType         elType;

  /* get the enclosing cell */
  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      /* move the selection at the beginning of the cell */
      if (TtaIsRowSaved (doc))
        {
          // move to the selection to the first cell in the row
          do
            {
              el = cell;
              TtaPreviousSibling (&el);
              if (el)
                cell = el;
            }
          while (el);
        }
      // move to the selection to the beginning of the cell
      child = cell;
      while (child)
        {
          el = child;
          child = TtaGetFirstChild (el);
        }
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
        TtaSelectString (doc, el, 1, 0);
      else
        TtaSelectElement (doc, el);
      TtcPaste (doc, view);
      TtaCloseUndoSequence (doc);
    }
}

/*----------------------------------------------------------------------
  PasteAfter
  ----------------------------------------------------------------------*/
void PasteAfter (Document doc, View view)
{
  Element             cell, el, child;
  ElementType         elType;
  int                 len;

  /* get the enclosing cell */
  cell = GetEnclosingCell (doc, FALSE, TRUE);
  if (cell)
    {
      /* move the selection at the end of the cell */
      child = cell;
      while (child)
        {
          el = child;
          child = TtaGetLastChild (el);
        }
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
        {
          len = TtaGetElementVolume (el);
          TtaSelectString (doc, el, len, len-1);
        }
      else
        TtaSelectElement (doc, el);
      TtcPaste (doc, view);
      TtaCloseUndoSequence (doc);
    }
}

/*----------------------------------------------------------------------
  IsBeforeRow returns the enclosing tbody and TRUE if the new element
  must be inserted before.
  Parameters head or foot is TRUE when the iserted element is a THead of
  a Tfoot.
  ----------------------------------------------------------------------*/
static Element BeginningOrEndOfTBody (Element cell, Document doc,
                                      ThotBool head, ThotBool foot,
                                      ThotBool *before)
{
  Element             tbody;
  ElementType         elType;

  tbody = NULL;
  if (cell)
    {
      elType = TtaGetElementType (cell);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* not an HTML Cell */
        return NULL;
      /* get the enclosing tbody */
      tbody = cell;
      do
        {
          tbody = TtaGetParent (tbody);
          if (tbody)
            elType = TtaGetElementType (tbody);
        }
      while (tbody &&
             (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") ||
              elType.ElTypeNum != HTML_EL_tbody));

      if (tbody == NULL)
        return NULL;
      if (head)
        {
          /* look for the first tbody */
          tbody = TtaGetParent (tbody);
          *before = TRUE;
        }
      else if (foot)
        {
          /* look for the last tbody */
          tbody = TtaGetParent (tbody);
          *before = FALSE;
        }
      else
        {
          do
            {
              TtaPreviousSibling (&cell);
              if (cell)
                elType = TtaGetElementType (cell);
            }
          while (cell &&
                 (strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") ||
                  (elType.ElTypeNum != HTML_EL_Data_cell &&
                   elType.ElTypeNum != HTML_EL_Heading_cell)));
          *before = (cell == NULL);
        }
      return tbody;
    }
  else
    return NULL;
}
 
/*----------------------------------------------------------------------
  CreateTHead
  ----------------------------------------------------------------------*/
void CreateTHead (Document doc, View view)
{
  Element             cell, el, elNew, row;
  ElementType         elType;
  DisplayMode         dispMode;
  NotifyElement       event;
  ThotBool            before;

  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      el = BeginningOrEndOfTBody (cell, doc, TRUE, FALSE, &before);
      if (el)
        {
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          /* generate a thead and its row */
          elType = TtaGetElementType (el);
          elType.ElTypeNum = HTML_EL_thead;
          elNew = TtaNewElement (doc, elType);
          elType.ElTypeNum = HTML_EL_Table_row;
          row = TtaNewElement (doc, elType);
          TtaInsertSibling (elNew, el, before, doc);
          TtaInsertFirstChild (&row, elNew, doc);
          event.element = row;
          event.document = doc;
          RowCreated (&event);
          TtaRegisterElementCreate (elNew, doc);
          el = TtaGetFirstLeaf (row);
          TtaSelectElement (doc, el);
          TtaSetDisplayMode (doc, dispMode);
        }
      TtaCloseUndoSequence (doc);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  CreateTBody
  ----------------------------------------------------------------------*/
void CreateTBody (Document doc, View view)
{
  Element             cell, el, elNew, row;
  ElementType         elType;
  DisplayMode         dispMode;
  NotifyElement       event;
  ThotBool            before;

  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      el = BeginningOrEndOfTBody (cell, doc, FALSE, FALSE, &before);
      if (el)
        {
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          /* generate a tbody and its row */
          elType = TtaGetElementType (el);
          elType.ElTypeNum = HTML_EL_tbody;
          elNew = TtaNewElement (doc, elType);
          elType.ElTypeNum = HTML_EL_Table_row;
          row = TtaNewElement (doc, elType);
          TtaInsertSibling (elNew, el, before, doc);
          TtaInsertFirstChild (&row, elNew, doc);
          event.element = row;
          event.document = doc;
          RowCreated (&event);
          TtaRegisterElementCreate (elNew, doc);
          el = TtaGetFirstLeaf (row);
          TtaSelectElement (doc, el);
          TtaSetDisplayMode (doc, dispMode);
        }
      TtaCloseUndoSequence (doc);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  CreateTFoot
  ----------------------------------------------------------------------*/
void CreateTFoot (Document doc, View view)
{
  Element             cell, el, elNew, row;
  ElementType         elType;
  DisplayMode         dispMode;
  NotifyElement       event;
  ThotBool            before;

  cell = GetEnclosingCell (doc, TRUE, TRUE);
  if (cell)
    {
      el = BeginningOrEndOfTBody (cell, doc, FALSE, TRUE, &before);
      if (el)
        {
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          /* generate a tfoot and its row */
          elType = TtaGetElementType (el);
          elType.ElTypeNum = HTML_EL_tfoot;
          elNew = TtaNewElement (doc, elType);
          elType.ElTypeNum = HTML_EL_Table_row;
          row = TtaNewElement (doc, elType);
          TtaInsertSibling (elNew, el, before, doc);
          TtaInsertFirstChild (&row, elNew, doc);
          event.element = row;
          event.document = doc;
          RowCreated (&event);
          TtaRegisterElementCreate (elNew, doc);
          el = TtaGetFirstLeaf (row);
          TtaSelectElement (doc, el);
          TtaSetDisplayMode (doc, dispMode);
        }
      TtaCloseUndoSequence (doc);
      TtaSetDocumentModified (doc);
    }
}

/*----------------------------------------------------------------------
  GetEnclosingForm creates if necessary and returns the	
  enclosing form element.				
  ----------------------------------------------------------------------*/
Element GetEnclosingForm (Document doc, View view)
{
  Element             el;
  ElementType         elType;
  int                 firstchar, lastchar;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  if (el)
    {
      /* there is a selection */
      elType = TtaGetElementType (el);
      while (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0
             && elType.ElTypeNum != HTML_EL_BODY
             && elType.ElTypeNum != HTML_EL_Form)
        {
          el = TtaGetParent (el);
          if (el == NULL)
            return (el);
          elType = TtaGetElementType (el);
        }
      if (elType.ElTypeNum != HTML_EL_Form)
        {
          /* it is not already a form */
          elType.ElTypeNum = HTML_EL_Form;
          TtaInsertElement (elType, doc);
          TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
        }
    }
  return (el);
}

/*----------------------------------------------------------------------
  PrepareFormControl 
  
  Return the selected element and make sure there is at least a space
  character before the insertion point.
  withinP is TRUE if the current selection is within a paragraph or
  a pseudo-paragraph, or Hi.
  ----------------------------------------------------------------------*/
static Element PrepareFormControl (Document doc, ThotBool *withinP)
{
  ElementType         elType;
  Element             el, parent;
  int                 firstchar, lastchar;
  int                 len;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  *withinP = FALSE;
  if (el)
    /* some element is selected */
    {
      /* check whether the selected element is within a P element */
      parent = el;
      while (parent && !IsBlockElement (parent))
        parent = TtaGetParent (parent);
      *withinP =  (parent != NULL);
      if (*withinP)
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == HTML_EL_Basic_Set ||
              elType.ElTypeNum == HTML_EL_Basic_Elem)
            /* the selected element is an undefined element. Make it
               a text leaf */
            {
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              parent = el;
              el = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&el, parent, doc);
            }
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            /* the selected element is a text leaf */
            {
              /* add a space if necessary */
              len = TtaGetTextLength (el);
              if (len == 0)
                {
                  TtaSetTextContent (el, (unsigned char*)" ",
                                     TtaGetDefaultLanguage (), doc);
                  /* set a caret after the new space */
                  TtaSelectString (doc, el, 2, 1);
                }
	       
            }
        }
    }
  return (el);
}

/*----------------------------------------------------------------------
  CreateForm
  Create a <form> element.
  ----------------------------------------------------------------------*/
void CreateForm (Document doc, View view)
{
  Element           el;
  int               firstchar, lastchar;
  ElementType       elType;
  ThotBool          oldStructureChecking;

  /* get the first selected element */
  TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  if (el == NULL)
    /* no selection */
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
  else
    /* there is some selection */
    {
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_Form;
      if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* the selection is within some HTML document or fragment */
        {
          /* create the form element */
          oldStructureChecking = TtaGetStructureChecking (doc);
          if (!oldStructureChecking)
            TtaSetStructureChecking (TRUE, doc);
          elType.ElTypeNum = HTML_EL_Form;
          TtaCreateElement (elType, doc);
          if (!oldStructureChecking)
            TtaSetStructureChecking (FALSE, doc);
        }
    }
}

/*----------------------------------------------------------------------
  CreateInputElement insert an input element:
  - within an existing paragraph generates input + text
  - in other case generates a paragraph including text + input + text
  ----------------------------------------------------------------------*/
static void CreateInputElement (Document doc, View view, int elInput)
{
  ElementType         elType;
  AttributeType       attrType;
  Element             el, input = NULL, parent;
  Attribute           attr;
  int                 firstchar, lastchar;
  ThotBool            withinP, oldStructureChecking;

  /* create the form if necessary */
  el = PrepareFormControl (doc, &withinP);
  if (el == NULL)
    /* no selection */
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
  else
    {
      /* the element can be created */
      elType = TtaGetElementType (el);
      if (!withinP)
        {
          /* create the pseudo paragraph element */
          elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
          TtaInsertElement (elType, doc);
          TtaGiveFirstSelectedElement (doc, &parent, &firstchar, &lastchar);
          /* check if the pseudo paragraph should becomes a paragraph */
          CheckPseudoParagraph (parent, doc);
          /* create the input element */
          elType.ElTypeNum = elInput;
          input = TtaNewTree (doc, elType, "");
          if (elInput == HTML_EL_Text_Area)
            AddRowsColumns (input, doc);
          TtaInsertFirstChild (&input, parent, doc);
          /* Insert a text element before */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          el = TtaNewElement (doc, elType);
          TtaInsertSibling (el, input, TRUE, doc);
        }
      else
        {
          /* create the input element */
          elType.ElTypeNum = elInput;
          oldStructureChecking = TtaGetStructureChecking (doc);
          if (elInput == HTML_EL_Text_Area)
            TtaSetStructureChecking (FALSE, doc);
          if (TtaInsertElement (elType, doc))
            {
              TtaGiveFirstSelectedElement (doc, &input, &firstchar, &lastchar);
              if (input)
                {
                  elType = TtaGetElementType (input);
                  while (input && elType.ElTypeNum != elInput)
                    {
                      input = TtaGetParent (input);
                      elType = TtaGetElementType (input);
                    }
                  if (input && (elInput == HTML_EL_Text_Area) && (elType.ElTypeNum == elInput))
                    AddRowsColumns (input, doc);
                  if (elInput == HTML_EL_Text_Area)
                    TtaSetStructureChecking (oldStructureChecking, doc);
                  /* add a text before if needed */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  el = input;
                  TtaPreviousSibling (&el);
                  if (el == NULL)
                    {
                      el = TtaNewElement (doc, elType);
                      TtaInsertSibling (el, input, TRUE, doc);
                    }
                }
            }
        }
      /* Insert a text element after */
      if (input)
        {
          el = input;
          TtaNextSibling (&el);
          if (el == NULL)
            {
              el = TtaNewElement (doc, elType);
              TtaInsertSibling (el, input, FALSE, doc);
              if (elInput == HTML_EL_Text_Input ||
                  elInput == HTML_EL_Password_Input ||
                  elInput == HTML_EL_File_Input)
                /* set the default size if there is no size attribute */
                {
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = HTML_ATTR_IntAreaSize;
                  attr = TtaGetAttribute (input, attrType);
                  if (!attr)
                    CreateAttrIntAreaSize (20, input, doc);
                }
              /* if it's not a HTML_EL_BUTTON_ or a SELECT
                 select the following text element */
              if (elInput != HTML_EL_BUTTON_ &&
                  elInput != HTML_EL_Option_Menu)
                TtaSelectElement (doc, el);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CreateFieldset
  ----------------------------------------------------------------------*/
void CreateFieldset (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_FIELDSET, doc);
}

/*----------------------------------------------------------------------
  CreateToggle
  ----------------------------------------------------------------------*/
void CreateToggle (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Checkbox_Input);
}

/*----------------------------------------------------------------------
  CreateRadio
  ----------------------------------------------------------------------*/
void CreateRadio (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Radio_Input);
}

/*----------------------------------------------------------------------
  UpdateAttrSelected
  ----------------------------------------------------------------------*/
void UpdateAttrSelected (NotifyAttribute * event)
{
  OnlyOneOptionSelected (event->element, event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrSelectedDeleted
  ----------------------------------------------------------------------*/
void AttrSelectedDeleted (NotifyAttribute * event)
{
  Element	menu;

  menu = TtaGetParent (event->element);
  OnlyOneOptionSelected (menu, event->document, FALSE);
}

/*----------------------------------------------------------------------
  DeleteAttrSelected
  ----------------------------------------------------------------------*/
ThotBool DeleteAttrSelected (NotifyAttribute * event)
{
  return TRUE;			/* refuse to delete this attribute */
}

/*----------------------------------------------------------------------
  CreateOption
  ----------------------------------------------------------------------*/
void CreateOption (Document doc, View view)
{
  ElementType         elType;
  Element             el, new_;
  int                 firstchar, lastchar;

  /* create the form if necessary */
  CreateInputElement (doc, view, HTML_EL_Option_Menu);
  TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
  if (el != NULL)
    {
      /* create the option */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_Option;
      new_ = TtaNewTree (doc, elType, "");
      TtaInsertFirstChild (&new_, el, doc);
      OnlyOneOptionSelected (new_, doc, FALSE);
      /* Select the text element within the option */
      el = TtaGetFirstChild (new_);
      TtaSelectElement (doc, el);
      TtaSelectView (doc, TtaGetViewFromName (doc, "Structure_view"));
    }
}

/*----------------------------------------------------------------------
  CreateOptGroup
  ----------------------------------------------------------------------*/
void CreateOptGroup (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_OptGroup, doc);
}

/*----------------------------------------------------------------------
  CreateTextInput
  ----------------------------------------------------------------------*/
void CreateTextInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Text_Input);
}

/*----------------------------------------------------------------------
  CreatePasswordInput
  ----------------------------------------------------------------------*/
void CreatePasswordInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Password_Input);
}

/*----------------------------------------------------------------------
  CreateTextArea
  ----------------------------------------------------------------------*/
void CreateTextArea (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Text_Area);
}


/*----------------------------------------------------------------------
  CreateImageInput
  ----------------------------------------------------------------------*/
void CreateImageInput (Document doc, View view)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  Element             el;
  char               *value;
  int                 length;
  int                 firstchar, lastchar;
  ThotBool            withinP, newAttr;

  /* create the form if necessary */
  el = PrepareFormControl (doc, &withinP);
  if (el != NULL)
    {
      /* the element can be created */
      elType = TtaGetElementType (el);
      if (!withinP)
        {
          /* create the paragraph element */
          elType.ElTypeNum = HTML_EL_Paragraph;
          TtaInsertElement (elType, doc);
          TtaExtendUndoSequence (doc);
        }
      ImgAlt[0] = EOS;
      AddNewImage (doc, view, TRUE, FALSE);
      TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
      if (el)
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
            {
              // select the enclosing input
              el = TtaGetParent (el);
              elType = TtaGetElementType (el);
            }
          if (elType.ElTypeNum == HTML_EL_Image_Input)
            /* the img element was created */
            {
              TtaExtendUndoSequence (doc);
              /* use the ALT value to generate the attribute NAME */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = HTML_ATTR_ALT;
              attr = TtaGetAttribute (el, attrType);
              if (attr)
                {
                  length = TtaGetTextAttributeLength (attr) + 10;
                  value = (char *)TtaGetMemory (length);
                  TtaGiveTextAttributeValue (attr, value, &length);
                  attrType.AttrTypeNum = HTML_ATTR_NAME;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr == NULL)
                    {
                      newAttr = TRUE;
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (el, attr, doc);
                    }
                  else
                    newAttr = FALSE;
                  TtaSetAttributeText (attr, value, el, doc);
                  if (newAttr)
                    TtaRegisterAttributeCreate (attr, el, doc);
                  else
                    TtaRegisterAttributeReplace (attr, el, doc);
                  TtaFreeMemory (value);
                  /* Check attribute NAME or ID in order to make sure that its
                     value unique in the document */
                  MakeUniqueName (el, doc, TRUE, FALSE);
                }
              TtaCloseUndoSequence (doc);
            }
        }
    }
}

/*----------------------------------------------------------------------
  CreateFileInput
  ----------------------------------------------------------------------*/
void CreateFileInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_File_Input);
}

/*----------------------------------------------------------------------
  CreateHiddenInput
  ----------------------------------------------------------------------*/
void CreateHiddenInput (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Hidden_Input);
}

/*----------------------------------------------------------------------
  CreateLabel
  ----------------------------------------------------------------------*/
void  CreateLabel (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_LABEL, doc);
}

/*----------------------------------------------------------------------
  CreatePushButton
  ----------------------------------------------------------------------*/
void  CreatePushButton (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_BUTTON_);
}

/*----------------------------------------------------------------------
  CreateSubmit
  ----------------------------------------------------------------------*/
void  CreateSubmit (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Submit_Input);
}

/*----------------------------------------------------------------------
  CreateReset
  ----------------------------------------------------------------------*/
void  CreateReset (Document doc, View view)
{
  CreateInputElement (doc, view, HTML_EL_Reset_Input);
}

/*----------------------------------------------------------------------
  CreateDivision
  ----------------------------------------------------------------------*/
void  CreateDivision (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Division, doc);
}

/*----------------------------------------------------------------------
  CreateNOSCRIPT
  ----------------------------------------------------------------------*/
void  CreateNOSCRIPT (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_NOSCRIPT, doc);
}

/*----------------------------------------------------------------------
  CreateIFrame
  ----------------------------------------------------------------------*/
void  CreateIFrame (Document doc, View view)
{
  ElementType         elType;
  Element             el, child;
  Attribute           attr;
  AttributeType       attrType;
  int                 firstchar, lastchar;
  ThotBool            oldStructureChecking;

  oldStructureChecking = TtaGetStructureChecking (doc);
  if (HTMLelementAllowed (doc))
    {
      /* Don't check mandatory attributes */
      TtaSetStructureChecking (FALSE, doc);
      elType.ElSSchema = TtaGetSSchema ("HTML", doc);
      elType.ElTypeNum = HTML_EL_IFRAME;
      if (TtaInsertElement (elType, doc))
        {
          /* Check the Thot abstract tree against the structure schema. */
          TtaSetStructureChecking (TRUE, doc);
          /* get the first selected element, i.e. the Object element */
          TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
          elType = TtaGetElementType (el);
          while (el != NULL &&
                 elType.ElTypeNum != HTML_EL_IFRAME)
            {
              el = TtaGetParent (el);
              elType = TtaGetElementType (el);
            }
          
          if (el)
            {
              TtaExtendUndoSequence (doc);
              /* copy SRC attribute */
              attrType.AttrSSchema = elType.ElSSchema;
              attrType.AttrTypeNum = HTML_ATTR_FrameSrc;
              attr = TtaGetAttribute (el, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (el, attr, doc);
                  TtaSetAttributeText (attr, "source.html", el, doc);
                  TtaRegisterAttributeCreate (attr, el, doc);
                }
              /* now create a child element */
              child = TtaGetLastChild (el);
              if (child == NULL)
                {
                  elType.ElTypeNum = HTML_EL_Iframe_Content;
                  child = TtaNewElement (doc, elType);
                  TtaInsertFirstChild (&child, el, doc);
                  TtaRegisterElementCreate (child, doc);
                }
              elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
              el = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&el, child, doc);
              TtaRegisterElementCreate (el, doc);
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              child = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&child, el, doc);
              TtaSelectElement (doc, child);
              TtaRegisterElementCreate (child, doc);
              TtaCloseUndoSequence (doc);
            }
        }
    }
  TtaSetStructureChecking (oldStructureChecking, doc);
}

/*----------------------------------------------------------------------
  CreateParameter
  ----------------------------------------------------------------------*/
void  CreateParameter (Document doc, View view)
{
  CreateHTMLelement (HTML_EL_Parameter, doc);
}

/*----------------------------------------------------------------------
  CreateOrChangeLink
  If current selection is within an anchor, change that link, otherwise
  create a link.
  ----------------------------------------------------------------------*/
void  CreateOrChangeLink (Document doc, View view)
{
  Element             el;
  ElementType         elType;
  Attribute           attr;
  char               *s;
  int                 firstSelectedChar, i;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;
  if (DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docText)
    return;
  UseLastTarget = FALSE;
  TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar, &i);

  if (el != NULL)
    {
      /* Look if there is an enclosing anchor element */
      el = SearchAnchor (doc, el, &attr, TRUE);
      if (el)
        {
          elType = TtaGetElementType (el);
          s = TtaGetSSchemaName (elType.ElSSchema);
          if ((elType.ElTypeNum != HTML_EL_Anchor || strcmp (s, "HTML"))
#ifdef _SVG
              && (elType.ElTypeNum != SVG_EL_a || strcmp (s, "SVG"))
#endif /* _SVG */
              )
            {
              /* it's not an anchor */
              if (TtaIsReadOnly (el))
                /* the selected element is read-only */
                return;
              el = NULL;
            }
          else if (TtaIsReadOnly (el))
           {
             if (!IsTemplateInstanceDocument (doc) ||
                 (!strcmp (s, "HTML") && !AllowAttributeEdit (el, doc, "href")) ||
                 (!strcmp (s, "SVG") && !AllowAttributeEdit (el, doc, "xlink:href")))
               /* the selected element is read-only */
               return;
            }
        }

      if (el == NULL)
        {
          /* The link element is a new created one */
          IsNewAnchor = TRUE;
          /* no anchor element, create a new link */
          CreateAnchor (doc, view, TRUE);
        }
      else
        {
          /* The link element already exists */
          IsNewAnchor = FALSE;
          /* There is an anchor. Just select a new destination */
          SelectDestination (doc, el, TRUE, FALSE);
        }
    }
  else
    TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
}

/*----------------------------------------------------------------------
  DoDeleteAnchor
  Delete the surrounding anchor.
  noCallback is TRUE when it's not a callback action
  ----------------------------------------------------------------------*/
void DoDeleteAnchor (Document doc, View view, ThotBool noCallback)
{
  Element             firstSelectedElement, lastSelectedElement;
  Element             anchor, child, next, previous;
  ElementType         elType;
  DisplayMode         dispMode;
  int                 firstSelectedChar, lastSelectedChar, i;

  if (!TtaGetDocumentAccessMode (doc))
    /* the document is in ReadOnly mode */
    return;

  if (noCallback)
    {
      /* get the first selected element */
      TtaGiveFirstSelectedElement (doc, &firstSelectedElement,
                                   &firstSelectedChar, &lastSelectedChar);
      if (firstSelectedElement == NULL)
        {
          /* no selection. Do nothing */
          TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
          return;
        }
      if (TtaIsReadOnly (firstSelectedElement))
        /* the selected element is read-only */
        return;

      elType = TtaGetElementType (firstSelectedElement);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
        /* the first selected element is not an HTML element. Do nothing */
        return;

      TtaGiveLastSelectedElement (doc, &lastSelectedElement, &i, 
                                  &lastSelectedChar);
      TtaOpenUndoSequence (doc, firstSelectedElement, lastSelectedElement,
                           firstSelectedChar, lastSelectedChar);
      anchor = firstSelectedElement;
    }
  else
    {
      anchor = AttrHREFelement;
      firstSelectedElement = lastSelectedElement = anchor;
      elType = TtaGetElementType (anchor);
      firstSelectedChar = 0;
    }

  if (elType.ElTypeNum == HTML_EL_Anchor)
    /* the first selected element is an anchor */
    {
      /* prepare the elements to be selected later */
      firstSelectedElement = TtaGetFirstChild (anchor);
      lastSelectedElement = TtaGetLastChild (anchor);
      firstSelectedChar = 0;
      lastSelectedChar = 0;
    }
  else if (elType.ElTypeNum == HTML_EL_AREA)
    {
      /* prepare the elements to be selected later */
      firstSelectedElement = NULL;
      lastSelectedElement = NULL;
      firstSelectedChar = 0;
      lastSelectedChar = 0;
    }
  else if (elType.ElTypeNum == HTML_EL_Cite)
    {
      anchor = NULL;
      SetOnOffCite (doc, 1);
      return;
    }
  else if (elType.ElTypeNum != HTML_EL_LINK)
    {
      /* search the surrounding Anchor element */
      elType.ElTypeNum = HTML_EL_Anchor;
      anchor = TtaGetTypedAncestor (firstSelectedElement, elType);
    }

  if (anchor)
    {
      /* ask Thot to stop displaying changes made in the document */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      TtaUnselect (doc);
      TtaRegisterElementDelete (anchor, doc);
      /* move all chidren of element anchor as sibling of the anchor */
      child = TtaGetFirstChild (anchor);
      previous = anchor;
      while (child != NULL)
        {
          next = child;
          TtaNextSibling (&next);
          TtaRemoveTree (child, doc);
          TtaInsertSibling (child, previous, FALSE, doc);
          TtaRegisterElementCreate (child, doc);
          previous = child;
          child = next;
        }
      /* prepare the next selection */
      if (firstSelectedElement == NULL)
        firstSelectedElement = TtaGetPredecessor (anchor);
      if (firstSelectedElement == NULL)
        firstSelectedElement = TtaGetSuccessor (anchor);
      if (firstSelectedElement == NULL)
        firstSelectedElement = TtaGetParent (anchor);
      /* delete the anchor element itself */
      if (anchor == AttrHREFelement)
        AttrHREFelement = NULL;
      TtaDeleteTree (anchor, doc);
      TtaSetStatus (doc, 1, " ", NULL);
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
    }

  if (noCallback)
    {
      TtaCloseUndoSequence (doc);
      TtaSetDocumentModified (doc);
    }

  /* set the selection */
  if (firstSelectedChar > 1)
    {
      if (firstSelectedElement == lastSelectedElement)
        i = lastSelectedChar;
      else
        i = TtaGetElementVolume (firstSelectedElement);
      TtaSelectString (doc, firstSelectedElement, firstSelectedChar, i);
    }
  else
    TtaSelectElement (doc, firstSelectedElement);
  if (firstSelectedElement != lastSelectedElement)
    TtaExtendSelection (doc, lastSelectedElement, lastSelectedChar);
}

/*----------------------------------------------------------------------
  DeleteAnchor
  Delete the surrounding anchor.                    
  ----------------------------------------------------------------------*/
void DeleteAnchor (Document doc, View view)
{
  DoDeleteAnchor (doc, view, TRUE);
}

/*----------------------------------------------------------------------
  ConfigAmaya
  A frontend to all Amaya configurations
  ----------------------------------------------------------------------*/
void ConfigAmaya (Document doc, View view)
{
  GeneralConfMenu (doc, view);
}

/*----------------------------------------------------------------------
  SaveOptions
  Saves the user modified configuration options
  ----------------------------------------------------------------------*/
void SaveOptions (Document doc, View view)
{
  TtaSaveAppRegistry ();
}

/*----------------------------------------------------------------------
  AnnotateDocument
  Frontend to the function that creates an annotation
  ----------------------------------------------------------------------*/
void AnnotateDocument (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Create (doc, view, ANNOT_useDocRoot);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  AnnotateDocument
  Frontend to the function that creates an annotation
  ----------------------------------------------------------------------*/
void AnnotateSelection (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Create (doc, view, ANNOT_useSelection);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  LoadAnnotations
  Frontend to the function that loads the annotations related to a document
  ----------------------------------------------------------------------*/
void LoadAnnotations (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Load (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  PostAnnotation
  Frontend to the function that posts an annotation to the server
  ----------------------------------------------------------------------*/
void PostAnnotation (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Post (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  DeleteAnnotation
  Frontend to the function that posts an annotation to the server
  ----------------------------------------------------------------------*/
void DeleteAnnotation (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Delete (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  FilterAnnot
  Show/Hide the annotations
  ----------------------------------------------------------------------*/
void FilterAnnot (Document doc, View view)
{
#ifdef ANNOTATIONS
  AnnotFilter (doc, view);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  MoveAnnotationXPtr
  Move an annotation to the value stored in the XPointer
  ----------------------------------------------------------------------*/
void  MoveAnnotationXPtr (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Move (doc, view, FALSE);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  MoveAnnotationSel
  Move an annotation in a document to the current selection
  ----------------------------------------------------------------------*/
void  MoveAnnotationSel (Document doc, View view)
{
#ifdef ANNOTATIONS
  ANNOT_Move (doc, view, TRUE);
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  Show/Hide the annotations
  ----------------------------------------------------------------------*/
void ReplyToAnnotation (Document doc, View view)
{
#ifdef ANNOTATIONS
  /* for testing threading on the selection */
  ANNOT_Create (doc, view, (AnnotMode)(ANNOT_useDocRoot | ANNOT_isReplyTo));
#endif /* ANNOTATIONS */
}

/*----------------------------------------------------------------------
  CustomQuery the annotations base
  ----------------------------------------------------------------------*/
void CustomQuery (Document doc, View view)
{
#ifdef ANNOTATIONS
  //CustomQueryMenuInit (doc, view);
#endif /* ANNOTATIONS */
}

/*------------ Begin: WebDAV Modifications by Manuele Kirsch -----------*/
/* Note: There is no #include "davlib.h" in this file to prevent
 *       circular references between the header files.
 */        


/*----------------------------------------------------------------------
  CopyLockInformation
  Get the lock information of the document 
  ----------------------------------------------------------------------*/
void CopyLockInformation (Document doc, View view)
{
#ifdef DAV
  DAVCopyLockInfo (doc, view);
#endif /* DAV */
}

/*----------------------------------------------------------------------
  LockUnlock
  A toggle that indicates whether the document is locked.
  ----------------------------------------------------------------------*/
void LockUnlock (Document doc, View view) 
{
#ifdef DAV
  DAVLockIndicator (doc, view);
#endif /* DAV */

}


/*------------ End: WebDAV Modifications by Manuele Kirsch -----------*/


/*----------------------------------------------------------------------
  SelectAll
  ----------------------------------------------------------------------*/
void SelectAll (Document doc, View view) 
{
  Element     el;
  ElementType elType;
  
  switch(DocumentTypes[doc])
  {
    case docHTML:
      elType.ElSSchema = TtaGetSSchema ("HTML", doc);
      elType.ElTypeNum = HTML_EL_BODY;
      el = TtaSearchTypedElement(elType, SearchInTree, TtaGetRootElement(doc));
      if(el)
          TtaSelectElement(doc, el);
      break;
    default:
      TtaSelectElement(doc, TtaGetRootElement(doc));
      break;
  }
}
