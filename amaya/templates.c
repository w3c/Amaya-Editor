/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Amaya browser functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
 *
 * Authors: I. Vatton, L. Bonameau, S. Bonhomme (INRIA)
 *
 */

/* Included headerfiles */


#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "document.h"
#include "view.h"
#ifdef TEMPLATES
#include "Template.h"
#endif /* TEMPLATES */

#include "AHTURLTools_f.h"
#include "appdialogue_wx.h"
#include "css_f.h"
#include "init_f.h"
#include "parser.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "tree.h"
#include "wxdialogapi_f.h"

/* content of the attr name of the meta tag defining the doc's destination URL */
#define META_TEMPLATE_NAME "AMAYA_TEMPLATE"
/* URL of the script providing templates (for reload) */
static char   *script_URL;


/*----------------------------------------------------------------------
  NewTemplate: Create the "new document from template's" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  char  *templateDir, *lang;
  char   s[MAX_LENGTH];
  ThotBool useAmayaDir = FALSE;

  templateDir = TtaGetEnvString ("TEMPLATES_DIRECTORY");
  if (templateDir == NULL || IsW3Path (templateDir))
    {
      useAmayaDir = TRUE;
      templateDir = (char *) TtaGetMemory (MAX_LENGTH);
      lang = TtaGetVarLANG ();
      // Amaya's templates directory
      sprintf (templateDir, "%s%ctemplates%c%s%c",
               TtaGetEnvString ("THOTDIR"), DIR_SEP, DIR_SEP, lang, DIR_SEP);
      if (!TtaDirExists (templateDir))
        // By default use the English templates directory
        sprintf (templateDir, "%s%ctemplates%cen%c",
                 TtaGetEnvString ("THOTDIR"), DIR_SEP, DIR_SEP, DIR_SEP);
    }
  TtaExtractName (DocumentURLs[doc], s, DocumentName);
  strcpy (DirectoryName, s);

  int window_id  = TtaGetDocumentWindowId( doc, view );
  ThotWindow p_window = (ThotWindow) TtaGetWindowFromId(window_id);
  ThotBool created = CreateNewTemplateDocDlgWX(BaseDialog + OpenTemplate,
                                               p_window,
                                               doc,
                                               TtaGetMessage (AMAYA, AM_NEW_TEMPLATE),
                                               templateDir,
                                               s);
  if (useAmayaDir)
    TtaFreeMemory (templateDir);

  if (created)
    {
      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + OpenTemplate, TRUE);
    }
#endif // TEMPLATES
}


/*----------------------------------------------------------------------
  OpenTemplateDocument: Process the meta of a template document,
  changes the URL, try to save it and warn the user if it cannot be
  saved.
  ----------------------------------------------------------------------*/
void OpenTemplateDocument (Document doc)
{
#ifdef TEMPLATES
  ElementType         metaElType;
  Element             metaEl;
  Attribute           metaAttr;
  AttributeType       metaAttrType;
  ThotBool            found = FALSE;
  int                 length;
  char               *utf8path, *path;
  char               *URLdir, *URLname;

  metaElType.ElSSchema = TtaGetDocumentSSchema (doc);
  metaElType.ElTypeNum = HTML_EL_META;
  metaAttrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  metaAttrType.AttrTypeNum = HTML_ATTR_meta_name;
  /* search the meta element */
  metaEl = TtaGetMainRoot (doc);
  path = NULL;
  do
    {
      metaEl = TtaSearchTypedElement (metaElType, SearchForward, metaEl);
      if (metaEl != NULL)
        {
          metaAttr = TtaGetAttribute (metaEl, metaAttrType); 
          if (metaAttr != NULL)
            {
              length = TtaGetTextAttributeLength (metaAttr) + 1;
              utf8path = (char *) TtaGetMemory (length);
              TtaGiveTextAttributeValue (metaAttr, utf8path, &length);
              path = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
                                                  TtaGetDefaultCharset ());
              TtaFreeMemory (utf8path);
              found = !strcmp (path, META_TEMPLATE_NAME);
              /* free previous path if necessary */
              TtaFreeMemory (path);
              path = NULL;
            }
        }
    }
  while (metaEl && !found);

  if (found)
    {
      /* get the url of the document */
      metaAttrType.AttrTypeNum = HTML_ATTR_meta_content; 
      metaAttr = TtaGetAttribute (metaEl, metaAttrType);
      if (metaAttr != NULL)
        {
          length = TtaGetTextAttributeLength (metaAttr) + 1;
          utf8path = (char *) TtaGetMemory (length);
          TtaGiveTextAttributeValue (metaAttr, utf8path, &length);
          path = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
                                              TtaGetDefaultCharset ());
          TtaFreeMemory (utf8path);
        }

      /* Delete the meta element */
      TtaDeleteTree (metaEl, doc);  
      if (script_URL == NULL)
        script_URL = TtaStrdup (DocumentURLs[doc]);
      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = TtaStrdup (path);
      DocumentMeta[doc]->method = CE_TEMPLATE;
      AddURLInCombobox (DocumentURLs[doc], NULL, TRUE);
      TtaSetTextZone (doc, 1, URL_list);
      TtaSetDocumentUnmodified (doc);
      
      /* set the document name and dir */
      URLname = strrchr (path, URL_SEP);
      if (URLname)
        {
          URLname[0] = EOS;
          URLname++;
          URLdir = path;
          TtaSetDocumentDirectory (doc, URLdir);
          TtaSetDocumentName (doc, URLname);
          /* SetBrowserEditor(doc); */ 
        }
      else
        {
          TtaSetDocumentDirectory (doc, "");
          TtaSetDocumentName (doc, path);
        }
      SetWindowTitle (doc, doc, 0);
    }
  TtaFreeMemory (path);
#endif /* TEMPLATES */
}
  
/*----------------------------------------------------------------------
  ReloadTemplateParams : restores the script URL and method into meta
  to reload a template
  ----------------------------------------------------------------------*/
void ReloadTemplateParams (char **docURL, ClickEvent *method)
{
  *method = CE_FORM_GET;
  TtaFreeMemory (*docURL);
  *docURL = TtaStrdup (script_URL); 
}


/*------------------------------------------------------------------------
  InitTemplateList : fills template list with HTML files in 
  the templates directory
  ----------------------------------------------------------------------*/
void InitTemplateList ()
{
#ifdef TEMPLATES
  int i, nb,len;
  unsigned char *urlstring,c;
  char          *app_home;
  FILE          *file;
  CHARSET       encoding;
  
  TtaFreeMemory(Template_list);
  
  urlstring = (unsigned char *) TtaGetMemory (MAX_LENGTH);
  
  /* open the file list_url.dat into APP_HOME directory */
  app_home = TtaGetEnvString ("APP_HOME");

  sprintf ((char *)urlstring, "%s%clist_url_utf8.dat", app_home, DIR_SEP);
  encoding = UTF_8;
  
  file = TtaReadOpen ((char *)urlstring);

  *urlstring = EOS;
  if (file)
    {
      /* get the size of the file */
      fseek (file, 0L, 2);	
      /* end of the file */
      Template_list_len = (ftell (file) * 4) + GetMaxURLList() + 4;
      Template_list = (char *)TtaGetMemory (Template_list_len);
      Template_list[0] = EOS;
      fseek (file, 0L, 0);	/* beginning of the file */
      /* initialize the list by reading the file */
      i = 0;
      nb = 0;
      while (TtaReadByte (file, &c))
        {
          if (c == '"')
            {
              len = 0;
              urlstring[len] = EOS;
              while (len < MAX_LENGTH && TtaReadByte (file, &c) && c != EOL)
                {
                  if (c == '"')
                    urlstring[len] = EOS;
                  else if (c == 13) /* Carriage return */
                    urlstring[len] = EOS;
                  else
                    urlstring[len++] = (char)c;
                }
              urlstring[len] = EOS;
              if (i > 0 && len)
                /* add an EOS between two urls */
                URL_list[i++] = EOS;
              if (len)
                {
                  nb++;
                  strcpy ((char *)&Template_list[i], (char *)urlstring);
                  i += len;
                }
            }
        }
      Template_list[i + 1] = EOS;
      TtaReadClose (file);
    }
  TtaFreeMemory (urlstring);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Insert the meta element identifying the template's instance
  ----------------------------------------------------------------------*/
void InsertInstanceMeta (Document newdoc)
{
#ifdef TEMPLATES
  Element head = TtaGetMainRoot (newdoc);
  Element meta;
  ElementType elType;
  AttributeType	attrType;
  Attribute attr;
  char *temp_id, *name;
  int length;
  ThotBool found = false;
  
  elType.ElSSchema = TtaGetSSchema ("HTML", newdoc);
  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement(elType, SearchInTree, head);
  if (head)
    {
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attrType.AttrSSchema = elType.ElSSchema;

      elType.ElTypeNum = HTML_EL_META;      
      /* Search the template_id meta in the template document */
      meta = TtaSearchTypedElement(elType, SearchInTree, head);
      while (meta && !found)
        {
          attr = TtaGetAttribute (meta, attrType);
          if (attr != NULL)
            {
              length = TtaGetTextAttributeLength (attr) + 1;
              name = (char *) TtaGetMemory (length);
              TtaGiveTextAttributeValue (attr, name, &length);
              if (!strcmp (name, "template_id"))
                {
                  /* The element is the template ID meta */
                  attrType.AttrTypeNum = HTML_ATTR_meta_content;
                  attr = TtaGetAttribute (meta, attrType);
                  
                  if (attr)
                    {
                      length = TtaGetTextAttributeLength (attr) + 1;
                      temp_id = (char *) TtaGetMemory (length);
                      TtaGiveTextAttributeValue (attr, temp_id, &length);
                      found = True;
                    }
                  TtaFreeMemory(name);
                }
            }
          meta = TtaSearchTypedElement(elType, SearchForward, meta);
        }
      /* now "temp_id" should contain the content of template_id meta */
      
      if (found)
        {
          meta = TtaNewElement (newdoc, elType);
          
          /* Create the "name" attribute */
          
          attrType.AttrTypeNum = HTML_ATTR_meta_name;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (meta, attr, newdoc);
          TtaSetAttributeText (attr, "template", meta, newdoc);

          /* Create the "content" attribute */
          attrType.AttrTypeNum = HTML_ATTR_meta_content;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (meta, attr, newdoc);
          TtaSetAttributeText (attr, temp_id, meta, newdoc);
          
          TtaInsertFirstChild (&meta, head, newdoc);
        }
    }  
#endif /*TEMPLATES*/
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void UnlockContentElements (Document doc, Element el)
{
#ifdef TEMPLATES
  ElementType elType;

  if (el) 
    {
      elType = TtaGetElementType (el);
      if (TtaIsLeaf (elType))
        /* It's a content element */
        TtaSetAccessRight (el, ReadWrite, doc);
      else
        {
          el = TtaGetFirstChild(el);
          while (el)
            {
              UnlockContentElements (doc,el);
              TtaNextSibling (&el);
            }
        }
    }
#endif /*TEMPLATES*/
}

/*----------------------------------------------------------------------
  CheckFreeAreas
  Parse the subtree from el, set read-write access to the child elements
  of  free_struct elements.
  ----------------------------------------------------------------------*/
static void CheckFreeAreas (Document doc, Element el)
{
#ifdef TEMPLATES
  ElementType  elType;
  char        *s;
  
  el = TtaGetFirstChild(el);
  while (el)
    {
      elType = TtaGetElementType (el);
      s = TtaGetSSchemaName (elType.ElSSchema);
      /* The element is not a leaf */
      if (!strcmp (s,"Template") &&
          elType.ElTypeNum == Template_EL_FREE_STRUCT)
        /* The element has a free structure */
        TtaSetAccessRight (el, ReadWrite, doc);
      else if (!strcmp (s,"Template") &&
               elType.ElTypeNum == Template_EL_FREE_CONTENT)
        /* The element has free content */
        UnlockContentElements (doc, el);
      else
        /* The element has a fixed structure */
        /* So we look for a free structure in
           the subtree */
        CheckFreeAreas(doc, el);
      TtaNextSibling (&el);
    }
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Load a template and create the instance file - update images and 
  stylesheets related to the template.
  ----------------------------------------------------------------------*/
int CreateInstanceOfTemplate (Document doc, char *templatename, char *docname,
                              DocumentType docType)
{
#ifdef TEMPLATES
  Element       el;
  char                *s;
  int           newdoc, len;
  ThotBool      stopped_flag;

  if (!IsW3Path (docname) && TtaFileExist (docname))
    {
      s = (char *)TtaGetMemory (strlen (docname) +
                                strlen (TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK)) + 2);
      sprintf (s, TtaGetMessage (AMAYA, AM_OVERWRITE_CHECK), docname);
      InitConfirm (0, 0, s);
      TtaFreeMemory (s);
      if (!UserAnswer)
        return 0;
    }

  if (InNewWindow || DontReplaceOldDoc)
    {
      newdoc = InitDocAndView (doc,
                               !DontReplaceOldDoc /* replaceOldDoc */,
                               InNewWindow, /* inNewWindow */
                               DocumentName, (DocumentType)docType, 0, FALSE,
                               L_Other, (ClickEvent)CE_ABSOLUTE);
      DontReplaceOldDoc = FALSE;
    }
  else
    {
      /* record the current position in the history */
      AddDocHistory (doc, DocumentURLs[doc], 
                     DocumentMeta[doc]->initial_url,
                     DocumentMeta[doc]->form_data,
                     DocumentMeta[doc]->method);
      newdoc = InitDocAndView (doc,
                               !DontReplaceOldDoc /* replaceOldDoc */,
                               InNewWindow, /* inNewWindow */
                               DocumentName, (DocumentType)docType, 0, FALSE,
                               L_Other, (ClickEvent)CE_ABSOLUTE);
    }

  if (newdoc != 0)
    {
      TtaExtractName (templatename, DirectoryName, DocumentName);
      /* Register the last used template directory */
      TtaSetEnvString ("TEMPLATES_DIRECTORY", DirectoryName, TRUE);
      LoadDocument (newdoc, templatename, NULL, NULL, CE_ABSOLUTE,
                    "", DocumentName, NULL, FALSE, &DontReplaceOldDoc);
      InsertInstanceMeta (newdoc);
      
      /* Update URLs of linked documents */
      SetRelativeURLs (newdoc, docname);
      
      /* change its URL */
      TtaFreeMemory (DocumentURLs[newdoc]);
      len = strlen (docname) + 1;
      DocumentURLs[newdoc] = TtaStrdup (docname);
      DocumentSource[newdoc] = 0;
      /* add the URI in the combobox string */
      AddURLInCombobox (docname, NULL, FALSE);
      TtaSetTextZone (newdoc, 1, URL_list);
      /* change its directory name */
      TtaSetDocumentDirectory (newdoc, DirectoryName);

      TtaSetDocumentModified (newdoc);
      W3Loading = 0;		/* loading is complete now */
      DocNetworkStatus[newdoc] = AMAYA_NET_ACTIVE;
      stopped_flag = FetchAndDisplayImages (newdoc, AMAYA_LOAD_IMAGE, NULL);
      if (!stopped_flag)
        {
          DocNetworkStatus[newdoc] = AMAYA_NET_INACTIVE;
          /* almost one file is restored */
          TtaSetStatus (newdoc, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED),
                        NULL);
        }
      /* check parsing errors */
      CheckParsingErrors (newdoc);
      /* Set elements access rights according to free_* elements */
      el = TtaGetMainRoot (newdoc);
      if (el)
        {
          TtaSetAccessRight (el, ReadOnly, newdoc);
          CheckFreeAreas (newdoc, el);
        }
    }
  BackupDocument = 0;
  return (newdoc);
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  Here we put all the actions to performs when we know a document 
  is an instance
  ----------------------------------------------------------------------*/
void LoadInstanceOfTemplate (Document doc)
{
#ifdef TEMPLATES
  ThotBool   show;
  Element    el = TtaGetMainRoot (doc);
  char      *css_url;
  PInfoPtr            pInfo;
  CSSInfoPtr css;

  el = TtaGetMainRoot (doc);
  TtaSetAccessRight (el, ReadOnly, doc);
  CheckFreeAreas (doc, el);
  TtaGetEnvBoolean ("SHOW_TEMPLATES", &show);
  css_url = (char *) TtaGetMemory (MAX_LENGTH);
  sprintf (css_url, "%s%camaya%chide_template.css", TtaGetEnvString ("THOTDIR"),DIR_SEP,DIR_SEP);
  if (!show)
    {
      /* We don't want to show the frames around template's elements,
         so we load a stylesheet with border: none for all template's 
         elements */
      
      LoadStyleSheet (css_url, doc, NULL, NULL, NULL, CSS_ALL, FALSE);
    }
  else
    {
      css = SearchCSS (doc, css_url, NULL, &pInfo);
      if (css)
        RemoveStyle (css_url, doc, TRUE, TRUE, NULL, CSS_EXTERNAL_STYLE);
    }
  TtaFreeMemory(css_url);
#endif /* TEMPLATES */
}


/*----------------------------------------------------------------------
  ThotBool isTemplateInstance (Document doc) 
  Return true if the document is an instance 
  ----------------------------------------------------------------------*/
ThotBool IsTemplateInstance (Document doc)
{
#ifdef TEMPLATES
  Element el = TtaGetMainRoot (doc);
  Element meta;
  ElementType metaType;
  AttributeType	attrType;
  Attribute attr;
  ThotBool found = false;
  char* name,* version;
  int length;
  
  /* metaType is first used to find a meta element */
  metaType.ElSSchema = TtaGetSSchema ("HTML", doc);
  metaType.ElTypeNum = HTML_EL_META;
  meta = TtaSearchTypedElement(metaType, SearchInTree, el);

  /* attrType will be used to find the name attribute of a meta */
  attrType.AttrSSchema = metaType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_meta_name;
  while (meta && !found)
    {
      attr = TtaGetAttribute (meta, attrType);
      if (attr != NULL)
        /* The element contains a name attribute */
        {
          length = TtaGetTextAttributeLength (attr) + 1;
          name = (char *) TtaGetMemory (length);
          TtaGiveTextAttributeValue (attr, name, &length);

          if (!strcmp(name, "template"))
            {
              /* The element is the template meta, the document is a template instance */
              attrType.AttrTypeNum = HTML_ATTR_meta_content;
              attr = TtaGetAttribute (meta, attrType);
              
              if (attr)
                {
                  length = TtaGetTextAttributeLength (attr) + 1;
                  version = (char *) TtaGetMemory (length);
                  TtaGiveTextAttributeValue (attr, version, &length);
                  
                  DocumentMeta[doc]->template_version = version;
                  TtaFreeMemory (version);
                  found = True;
                }
            }
          TtaFreeMemory(name);
        }
      meta = TtaSearchTypedElement(metaType, SearchForward, meta);
    }
  
  return found;
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */
}
