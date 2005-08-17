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
#include "wxdialogapi_f.h"
#include "appdialogue_wx.h"
#include "init_f.h"
#include "parser.h"
#include "tree.h"
#include "HTMLimage_f.h"
#ifdef TEMPLATES
#include "Template.h"
#endif /* TEMPLATES */
#include "HTMLsave_f.h"

/* content of the attr name of the meta tag defining the doc's destination URL */
#define META_TEMPLATE_NAME "AMAYA_TEMPLATE"
/* URL of the script providing templates (for reload) */
static char   *script_URL;
#include "init_f.h"


/*----------------------------------------------------------------------
   NewTemplate: Create the "new document from template's" dialog
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
#ifdef TEMPLATES
  char *templateDir ;
  char *amaya_home ;

  templateDir = (char *) TtaGetMemory (MAX_LENGTH);
  amaya_home = TtaGetEnvString ("THOTDIR");

  // Amaya's templates directory (only french yet)
  sprintf ((char *)templateDir, "%s%ctemplates%cfr%c", amaya_home,DIR_SEP,DIR_SEP,DIR_SEP);
  
  char s [MAX_LENGTH];
  
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
}

/*-------------------------------------------------
 Insert the meta element identifying the template's
 instance
----------------------------------------------------*/
void insertTemplateMeta (Document newdoc)
{
  Element el = TtaGetMainRoot (newdoc);
  Element meta;
  Element head;
  ElementType elType;
  AttributeType	attrType;
  Attribute attr;
  
  elType.ElSSchema = TtaGetSSchema ("HTML", newdoc);
  elType.ElTypeNum = HTML_EL_HEAD;
  head = TtaSearchTypedElement(elType, SearchInTree, el);
  if (head)
    {
      el = TtaGetFirstChild(head);
      elType.ElTypeNum = HTML_EL_META;
      meta = TtaNewElement (newdoc, elType);

      /* Create the "name" attribute */
      attrType.AttrTypeNum = HTML_ATTR_meta_name;
      attrType.AttrSSchema = elType.ElSSchema;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, newdoc);
      TtaSetAttributeText (attr, "template", meta, newdoc);

      /* Create the "content" attribute */
      attrType.AttrTypeNum = HTML_ATTR_meta_content;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (meta, attr, newdoc);
      TtaSetAttributeText (attr, "version-modele", meta, newdoc);
      
      TtaInsertFirstChild (&meta, head, newdoc);
    }
  
}

/*-----------------------------------------------
void UnlockSubtree
Set read/write access to an element and all his
children
-----------------------------------------------*/

void UnlockSubtree (Document doc, Element el)
{
  TtaSetAccessRight (el, ReadWrite, doc);
  TtaNextSibling (&el);
  if (el != NULL)
    /* The element has at least one sibling */
    UnlockSubtree (doc, el);
}

void UnlockContentElements (Document doc, Element el)
{
  ElementType elType;

  if (el != NULL) 
    {
      elType = TtaGetElementType (el);
      if (TtaIsLeaf (elType))
        {
          /* It's a content element */
          TtaSetAccessRight(el, ReadWrite, doc);
        }
      else
        {
          TtaSetAccessRight(el, ReadOnly, doc);
          UnlockContentElements(doc,TtaGetFirstChild(el));
        }
      TtaNextSibling(&el);
      if (el != NULL)
        {
          UnlockContentElements(doc,el);
        }
    }
}

/*-----------------------------------------------
void LockFixedAreas
Parse the subtree from el, set read-only access to
the element and his child when it's not a free_struct
element.
------------------------------------------------*/

void LockFixedAreas (Document doc, Element el)
{
#ifdef TEMPLATES
  ElementType elType;
  char *s;
  
  TtaSetAccessRight (el, ReadOnly, doc);
  elType = TtaGetElementType(el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (TtaGetFirstChild(el)!=NULL)
    {
      /* The element is not a leaf */
      if ((strcmp (s,"Template") == 0) &&
          (elType.ElTypeNum == Template_EL_FREE_STRUCT))
        {
          /* The element has a free structure */
          UnlockSubtree (doc, el);
        }
      else if ((strcmp (s,"Template") == 0) &&
               (elType.ElTypeNum == Template_EL_FREE_CONTENT))
        {
          /* The element has free content */
          UnlockContentElements (doc, el);
        }
      else 	  
        {
          /* The element has a fixed structure */
          /* So we look for a free structure in
             the subtree */
          LockFixedAreas (doc, TtaGetFirstChild (el));
        }
    }
  TtaNextSibling (&el);
  if (el != NULL)
    /* The element has at least one sibling */
    LockFixedAreas (doc, el);
#endif /* TEMPLATES */
}

/*---------------------------------------------------------------
  Load a template and create the instance file - update images and 
  stylesheets related to the template.
  ---------------------------------------------------------------*/

int CreateInstanceOfTemplate (Document doc, char *templatename, char *docname,
			      DocumentType docType)
{
  char          templateFile[MAX_LENGTH];
  int           newdoc, len;
  ThotBool      stopped_flag;

  W3Loading = doc;
  BackupDocument = doc;
  TtaExtractName (templatename, DirectoryName, DocumentName);
  AddURLInCombobox (docname, NULL, TRUE);
  newdoc = InitDocAndView (doc,
                           !DontReplaceOldDoc /* replaceOldDoc */,
                           InNewWindow /* inNewWindow */,
                           DocumentName, (DocumentType)docType, 0, FALSE,
                           L_Other, (ClickEvent)CE_ABSOLUTE);
  if (newdoc != 0)
    {
      /* load the saved file */
      W3Loading = newdoc;
      
      templateFile[0] = EOS;
      
      LoadDocument (newdoc, templatename, NULL, NULL, CE_ABSOLUTE,
                    "", DocumentName, NULL, FALSE, &DontReplaceOldDoc);
      
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
      
      /* Set elements access rights
         according to free_* elements */
      Element el = TtaGetMainRoot (newdoc);
      LockFixedAreas (newdoc, el);

      insertTemplateMeta(newdoc);
    }
   BackupDocument = 0;
   return (newdoc);

}

void LoadInstanceOfTemplate (Document doc)
{
  Element el = TtaGetMainRoot (doc);
  LockFixedAreas (doc, el);
}
