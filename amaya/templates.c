/*
 *
 *  COPYRIGHT MIT and INRIA, 1996-2001.
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
  char      *url;

  url = TtaGetEnvString ("TEMPLATE_URL");
  if (url)
    GetHTMLDocument (url, NULL, 0, 0, CE_ABSOLUTE, FALSE, NULL, NULL);
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
  ThotBool            ok = FALSE;
  int                 length;
  char              buffer[MAX_LENGTH];
  char             *URLdir;
  char             *URLname;

  metaElType.ElSSchema = TtaGetDocumentSSchema (doc);
  metaElType.ElTypeNum = HTML_EL_META;
  metaAttrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  metaAttrType.AttrTypeNum = HTML_ATTR_meta_name;
  /* search the meta element */
  metaEl = TtaGetMainRoot (doc);
  do
    {
      metaEl = TtaSearchTypedElement (metaElType, SearchForward, metaEl);
      if (metaEl != NULL)
        {
          metaAttr = TtaGetAttribute (metaEl, metaAttrType); 
          if (metaAttr != NULL)
            {
              length = MAX_LENGTH - 1;
 	      TtaGiveTextAttributeValue (metaAttr, buffer, &length);
              found = !strcmp (buffer, META_TEMPLATE_NAME);
            }
        }
    }
  while (metaEl != NULL && !found);
  if (found)
    {
      /* get the url of the document */
      metaAttrType.AttrTypeNum = HTML_ATTR_meta_content; 
      metaAttr = TtaGetAttribute (metaEl, metaAttrType);
      if (metaAttr != NULL)
	{
	  length = MAX_LENGTH - 1;
	  TtaGiveTextAttributeValue (metaAttr, buffer, &length);
	}
      /* Delete the meta element */
      TtaDeleteTree (metaEl, doc);  
      /* try to save to the new url */
#ifdef WITH_PRELABLE_SAVE_POUR_VOIR      
      ok = SaveDocumentThroughNet (doc, 1, buffer, FALSE, TRUE, TRUE);
#else
      ok = TRUE;
#endif
      if (ok)
        {
          if (script_URL == NULL)
            script_URL = TtaStrdup (DocumentURLs[doc]);
	  TtaFreeMemory (DocumentURLs[doc]);
	  DocumentURLs[doc] = TtaStrdup (buffer);
          DocumentMeta[doc]->method = CE_TEMPLATE;
	  TtaSetTextZone (doc, 1, 1, DocumentURLs[doc]);
	  TtaSetDocumentUnmodified (doc);
          
	}
      else 
	{
	  /* save failed : print warning message */
         
	}
	
      /* set the document name and dir */
      
      URLname = strrchr (buffer, URL_SEP);
      if (URLname)
	{
	  URLname[0] = EOS;
	  URLname++;
	  URLdir = buffer;
	  TtaSetDocumentDirectory (doc, URLdir);
	  TtaSetDocumentName (doc, URLname);
	  /* SetBrowserEditor(doc); */ 
	}
      else
	{
	  TtaSetDocumentDirectory (doc, "");
	  TtaSetDocumentName (doc, buffer);
	}
      SetWindowTitle (doc, doc, 0);
    }
    
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
