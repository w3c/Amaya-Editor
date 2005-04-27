/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2004
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


/* content of the attr name of the meta tag defining the doc's destination URL */
#define META_TEMPLATE_NAME "AMAYA_TEMPLATE"
/* URL of the script providing templates (for reload) */
static char   *script_URL;
#include "init_f.h"


/*----------------------------------------------------------------------
   NewTemplate: Opens a template form on the remote template server
  ----------------------------------------------------------------------*/
void NewTemplate (Document doc, View view)
{
  char       *url;

  url = TtaGetEnvString ("TEMPLATE_URL");
  if (url && TtaCheckDirectory (url))
    {
      /*TtaNewScrollPopup (BaseDialog + xxx, TtaGetViewFrame (doc, 1),
			 NULL, nbitems, FormBuf, NULL, multipleOptions, 'L');
      InitOpenDocForm (doc, view, "New.html",
      "xxx", docHTML);*/
      /*GetAmayaDoc (url, NULL, 0, 0, CE_ABSOLUTE, FALSE, NULL, NULL);*/
    }
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
