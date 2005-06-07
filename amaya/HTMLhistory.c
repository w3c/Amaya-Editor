/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint, J. Kahan, I. Vatton
 *
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */
 
#define THOT_EXPORT extern
#include "amaya.h"
#include "helpmenu.h"
#include "css.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#
#include "AHTURLTools_f.h"
#include "HTMLhistory_f.h"
#include "HTMLsave_f.h"
#include "init_f.h"

#ifdef _WX
  #include "wx/msgdlg.h"
  #include "message_wx.h"
#endif /* _WX */

#define DOC_HISTORY_SIZE 32

/* a record in an history */
typedef struct _HistElement
{
  char    *HistUrl;        /* document URL */
  char    *HistInitialUrl; /* document URL */
  char    *form_data;      /* data associated with forms */
  int      method;         /* method used to request this URL */
  int      HistPosition;   /* volume preceding the first element to be
			      made visible in the main window */
  int      HistDistance;   /* distance from the  top of the window to the
			      top of this element (% of the window height) */
} HistElement;

/* the history of a window */
typedef HistElement   anHistory[DOC_HISTORY_SIZE];


/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _GotoHistory_context
{
  Document             doc;
  int                  prevnext;
  ThotBool	       last;
  ThotBool             next_doc_loaded;
  char                *initial_url;
} GotoHistory_context;

/* the history of all windows */
static anHistory    DocHistory[DocumentTableLength];
/* current position in the history of each window */
static int          DocHistoryIndex[DocumentTableLength];
static ThotBool     Back_Forward = FALSE;

/*----------------------------------------------------------------------
   InitDocHistory
   Reset history for document doc
  ----------------------------------------------------------------------*/
void InitDocHistory (Document doc)
{
   DocHistoryIndex[doc] = -1;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FreeDocHistory ()
{
  int               doc, i;

  for (doc = 0; doc < DocumentTableLength; doc++)
    {
      if (DocHistoryIndex[doc] > 0)
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	  {
	    if (DocHistory[doc][i].HistUrl != NULL)
	      TtaFreeMemory (DocHistory[doc][i].HistUrl);
	    if (DocHistory[doc][i].form_data != NULL)
	      TtaFreeMemory (DocHistory[doc][i].form_data);
	  }
    }
}
/*----------------------------------------------------------------------
  ElementAtPosition
  Returns the element that is at position pos in document doc.
  ----------------------------------------------------------------------*/
Element ElementAtPosition (Document doc, int pos)
{
   Element	el, result, child, next;
   int		sum, vol;
   ThotBool	stop;

   sum = 0;
   result = NULL;
   el = TtaGetMainRoot (doc);
   while (el != NULL && !result)
      {
      if (sum >= pos)
	result = el;
      else
	{
        child = TtaGetFirstChild (el);
	if (child == NULL)
	   result = el;
	else
	   {
	   el = child;
	   stop = FALSE;
	   do
	     {
	     vol = TtaGetElementVolume (el);
	     if (sum + vol <= pos)
	        {
		next = el;
	        TtaNextSibling (&next);
		if (next == NULL)
		  stop = TRUE;
		else
		  {
		  el = next;
	          sum += vol;
		  }
	        }
	     else
	        stop = TRUE;
	     }
           while (el != NULL && !stop);
	  }
        }
      }
   if (result)
      /* return the first leaf in the element found */
      {
      do
	{
	child = TtaGetFirstChild (result);
	if (child != NULL)
	   result = child;
	}
      while (child != NULL);
      }
   return result;
}

/*----------------------------------------------------------------------
  RelativePosition
  Returns the position of the first visible element in the main view of
  document doc.
  ----------------------------------------------------------------------*/
int RelativePosition (Document doc, int *distance)
{
   int		sum;
   Element	el, sibling, ancestor;

   sum = 0;
   el = TtaGetFirstElementShown (doc, 1, distance);
   ancestor = el;
   while (ancestor != NULL)
      {
      sibling = ancestor;
      do
	 {
         TtaPreviousSibling (&sibling);
	 if (sibling != NULL)
	    sum += TtaGetElementVolume (sibling);
	 }
      while (sibling != NULL);
      ancestor = TtaGetParent (ancestor);
      }
   return sum;
}

/*----------------------------------------------------------------------
  IsNextDocLoaded
  A IsDocumentLoaded frontend which returns TRUE if a given URL is already
  being displayed in another window. 
  ----------------------------------------------------------------------*/
static ThotBool IsNextDocLoaded (const Document baseDoc, const char *url,
				 const char *form_data, const ClickEvent CE_event)
{
  char              *tempdocument;
  char              *target;
  char              *documentname;
  char              *parameters;
  char              *pathname;
  int                loaded;

  if (url == NULL)
    return FALSE;

  tempdocument = (char *)TtaGetMemory (MAX_LENGTH);
  target = (char *)TtaGetMemory (MAX_LENGTH);
  documentname = (char *)TtaGetMemory (MAX_LENGTH);
  parameters = (char *)TtaGetMemory (MAX_LENGTH);
  pathname = (char *)TtaGetMemory (MAX_LENGTH);

  strcpy (tempdocument, (char *) url);
  ExtractParameters (tempdocument, parameters);
  /* Extract the target if necessary */
  ExtractTarget (tempdocument, target);

  /* Add the  base content if necessary */
  if (CE_event == CE_RELATIVE || CE_event == CE_FORM_GET
      || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
    NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
  else
    NormalizeURL (tempdocument, 0, pathname, documentname, NULL);

   /* check if the user is already browsing the document in another window */
   if (CE_event == CE_FORM_GET || CE_event == CE_FORM_POST)
     {
       loaded = IsDocumentLoaded (pathname, (char *) form_data);
       /* we don't concatenate the new parameters as we give preference
	  to the form data */
     }
   else
     {
       /* concatenate the parameters before making the test */
       if (parameters[0] != EOS)
	 {
	   strcat (pathname, "?");
	   strcat (pathname, parameters);
	 }
       loaded = IsDocumentLoaded (pathname, NULL);
     }

  TtaFreeMemory (pathname);
  TtaFreeMemory (tempdocument);
  TtaFreeMemory (target);
  TtaFreeMemory (documentname);
  TtaFreeMemory (parameters);

  return (loaded != 0);
}

/*----------------------------------------------------------------------
   GotoPreviousHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoPreviousHTML_callback (int newdoc, int status, char *urlName,
				char *outputfile, AHTHeaders *http_headers,
				void * context)
{
  Document             doc;
  Element	       el;
  GotoHistory_context *ctx = (GotoHistory_context *) context;
  int                  prev;

  if (ctx == NULL)
    {
      /* out of the critic section */
      Back_Forward = FALSE;
      return;
    }

  prev = ctx->prevnext;
  doc = ctx->doc;
  if (doc == newdoc && DocHistoryIndex[doc] == prev)
    {
      /* we are still working on the same document */
      /* show the document at the position stored in the history */
      el = ElementAtPosition (doc, DocHistory[doc][prev].HistPosition);
      TtaShowElement (doc, 1, el, DocHistory[doc][prev].HistDistance);

      /* set the Forward button on if it was the last document in the history */
      if (ctx->last)
	SetArrowButton (doc, FALSE, TRUE);
    }
  if (!DocumentMeta[doc]->initial_url)
    DocumentMeta[doc]->initial_url = ctx->initial_url;
  else
    TtaFreeMemory (ctx->initial_url);
  TtaFreeMemory (ctx);
  /* out of the critic section */
  Back_Forward = FALSE;
}

/*----------------------------------------------------------------------
   GotoPreviousHTML
   This function is called when the user presses the Previous button
  ----------------------------------------------------------------------*/
void GotoPreviousHTML (Document doc, View view)
{
  GotoHistory_context *ctx;
  char                *url = NULL;
  char                *initial_url = NULL;
  char                *form_data = NULL;
  int                  prev, i;
  int                  method;
  ThotBool	       last = FALSE;
  ThotBool             hist = FALSE;
  ThotBool             same_form_data;
  ThotBool             next_doc_loaded = FALSE;

  if (doc < 0 || doc >= DocumentTableLength)
    return;
  if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
    return;
  if (Back_Forward)
    /* a back/forward is already active */
    return;
  else
    /* enter the critic section */
    Back_Forward = TRUE;

  /* previous document in history */
  prev = DocHistoryIndex[doc];
  if (prev ==  0)
    prev = DOC_HISTORY_SIZE - 1;
  else
    prev--;
 
  /* nothing to do if there is no previous document */
  if (DocHistory[doc][prev].HistUrl == NULL)
    {
      /* out of the critic section */
      Back_Forward = FALSE;
      return;
    }
  
  /* get the previous document information*/
  url = DocHistory[doc][prev].HistUrl;
  initial_url = DocHistory[doc][prev].HistInitialUrl;
  form_data = DocHistory[doc][prev].form_data;
  method = DocHistory[doc][prev].method;

  /* is it the same form_data? */
  if (!form_data && (!DocumentMeta[doc]  || !DocumentMeta[doc]->form_data))
    same_form_data = TRUE;
  else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	   && (!strcmp (form_data, DocumentMeta[doc]->form_data)))
    same_form_data = TRUE;
  else
    same_form_data = FALSE;
  
  /* if the document has been edited, ask the user to confirm, except
     if it's simply a jump in the same document */
  if (DocumentURLs[doc] &&
      (strcmp(DocumentURLs[doc], url) || !same_form_data))
    {
      /* is the next document already loaded? */
      next_doc_loaded = IsNextDocLoaded (doc, url, form_data, (ClickEvent)method);
      if (!next_doc_loaded && !CanReplaceCurrentDocument (doc, view))
	{
	  /* out of the critic section */
	  Back_Forward = FALSE;
	  return;
	}
    }

  if (!next_doc_loaded)
    {
      /* the current document must be put in the history if it's the last
	 one */
      if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
	{
	  if ((DocumentURLs[doc] &&
	       !IsW3Path (DocumentURLs[doc]) &&
	       !TtaFileExist (DocumentURLs[doc])) ||
	      (DocumentURLs[doc] == NULL))
	    {
	      /* cannot store the current document in the history */
	      last = FALSE;
	      hist = FALSE;
	    }
	  else
	    {
	      last = TRUE;
	      hist = TRUE;
	    }
	}
      else
	{
	  i = DocHistoryIndex[doc];
	  i++;
	  i %= DOC_HISTORY_SIZE;
	  if (DocHistory[doc][i].HistUrl == NULL)
	    last = TRUE;
	}
      
      /* set the Back button off if there is no previous document in history */
      i = prev;
      if (i ==  0)
	i = DOC_HISTORY_SIZE - 1;
      else
	i--;
      if (DocHistory[doc][i].HistUrl == NULL)
	/* there is no previous document, set the Back button OFF */
	SetArrowButton (doc, TRUE, FALSE);
    }

  /* save the context */
  ctx = (GotoHistory_context*)TtaGetMemory (sizeof (GotoHistory_context));
  ctx->prevnext = prev;
  ctx->last = last;
  ctx->doc = doc;
  ctx->next_doc_loaded = next_doc_loaded;
  ctx->initial_url = TtaStrdup (initial_url);

  /* 
  ** load (or jump to) the previous document 
  */
  if (!next_doc_loaded)
    {
      if (hist)
	/* record the current position in the history */
	AddDocHistory (doc, DocumentURLs[doc], DocumentMeta[doc]->initial_url,
		       DocumentMeta[doc]->form_data, DocumentMeta[doc]->method);
      
      DocHistoryIndex[doc] = prev;
    }
  
  /* is it the current document ? */     
  if (DocumentURLs[doc] && !strcmp (url, DocumentURLs[doc]) && same_form_data)
    /* it's just a move in the same document */
    GotoPreviousHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
  else
    {
      StopTransfer (doc, 1);
      GetAmayaDoc (url, form_data, doc, doc, (ClickEvent)method, FALSE,
		   (void (*)(int, int, char*, char*, const AHTHeaders*, void*))
		   GotoPreviousHTML_callback,(void *) ctx);
      /* out of the critic section */
      Back_Forward = FALSE;
    }
}

/*----------------------------------------------------------------------
   GotoNextHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoNextHTML_callback (int newdoc, int status, char *urlName,
			    char *outputfile, AHTHeaders *http_headers,
			    void * context)
{
  Element	       el;
  Document             doc;
  GotoHistory_context     *ctx = (GotoHistory_context *) context;
  int                  next;
  
  /* retrieve the context */

  if (ctx == NULL)
    {
      /* out of the critic section */
      Back_Forward = FALSE;
      return;
    }

  next = ctx->prevnext;
  doc = ctx->doc;
  if (doc == newdoc && DocHistoryIndex[doc] == next)
    {
      /* we are still working on the same document */
      /* show the document at the position stored in the history */
      el = ElementAtPosition (doc, DocHistory[doc][next].HistPosition);
      TtaShowElement (doc, 1, el, DocHistory[doc][next].HistDistance);
    }
  if (!DocumentMeta[doc]->initial_url)
    DocumentMeta[doc]->initial_url = ctx->initial_url;
  else
    TtaFreeMemory (ctx->initial_url);
  TtaFreeMemory (ctx);
  /* out of the critic section */
  Back_Forward = FALSE;
}

/*----------------------------------------------------------------------
   GotoNextHTML
   This function is called when the user presses the Next button
  ----------------------------------------------------------------------*/
void GotoNextHTML (Document doc, View view)
{
  GotoHistory_context  *ctx;
  char                 *url = NULL;
  char                 *initial_url = NULL;
  char                 *form_data = NULL;
  int                   method;
  int		         next, i;
  ThotBool              same_form_data;
  ThotBool              next_doc_loaded = FALSE;

  if (doc < 0 || doc >= DocumentTableLength)
    return;
  if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
    return;
  if (Back_Forward)
    /* a back/forward is already active */
    return;
  else
    /* enter the critic section */
    Back_Forward = TRUE;

  /* next entry in history */
  next = DocHistoryIndex[doc] + 1;
  next %= DOC_HISTORY_SIZE;
  
  /* nothing to do if there is no next document */
  if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL ||
      DocHistory[doc][next].HistUrl == NULL)
    {
      /* out of the critic section */
      Back_Forward = FALSE;
      return;
    }

  /* Get the next document information */
  url = DocHistory[doc][next].HistUrl;
  initial_url = DocHistory[doc][next].HistInitialUrl;
  form_data = DocHistory[doc][next].form_data;
  method = DocHistory[doc][next].method;

  /* is the form_data the same? */
  if (!form_data && (!DocumentMeta[doc] || !DocumentMeta[doc]->form_data))
    same_form_data = TRUE;
  else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	   && (!strcmp (form_data, DocumentMeta[doc]->form_data)))
    same_form_data = TRUE;
  else
    same_form_data = FALSE;

  /* if the document has been edited, ask the user to confirm, except
     if it's simply a jump in the same document */
  if (DocumentURLs[doc] != NULL &&
      (strcmp (DocumentURLs[doc], DocHistory[doc][next].HistUrl) ||
       !same_form_data))
    {
      /* is the next document already loaded? */
      next_doc_loaded = IsNextDocLoaded (doc, url, form_data, (ClickEvent)method);
      if (!CanReplaceCurrentDocument (doc, view))
	{
	  /* out of the critic section */
	  Back_Forward = FALSE;
	  return;
	}
    }

  if (!next_doc_loaded)
    {
      /* set the Back button on if it's off */
      i = DocHistoryIndex[doc];
      if (i ==  0)
	i = DOC_HISTORY_SIZE - 1;
      else
	i--;
      if (DocHistory[doc][i].HistUrl == NULL)
	/* there is no document before the current one. The Back button is
	   normally OFF */
	/* set the Back button ON */
	SetArrowButton (doc, TRUE, TRUE);

      /* set the Forward button off if the next document is the last one
	 in the history */
      i = next;
      i++;
      i %= DOC_HISTORY_SIZE;
      if (DocHistory[doc][i].HistUrl == NULL)
	SetArrowButton (doc, FALSE, FALSE);
    }
   
  /*
  ** load the next document
  */
  if (!next_doc_loaded)
    DocHistoryIndex[doc] = next;

  /* save the context */
  ctx = (GotoHistory_context*)TtaGetMemory (sizeof (GotoHistory_context));
  ctx->prevnext = next;
  ctx->doc = doc;
  ctx->next_doc_loaded = next_doc_loaded;
  ctx->initial_url = TtaStrdup (initial_url);

  /* is it the current document ? */
  if (DocumentURLs[doc] && !strcmp (url, DocumentURLs[doc]) && same_form_data)
    /* it's just a move in the same document */
    GotoNextHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
  else
    {
      StopTransfer (doc, 1);
      GetAmayaDoc (url, form_data, doc, doc, (ClickEvent)method, FALSE,
		   (void (*)(int, int, char*, char*, const AHTHeaders*, void*)) GotoNextHTML_callback, (void *) ctx);
      /* out of the critic section */
      Back_Forward = FALSE;
    }
}

/*----------------------------------------------------------------------
   AddDocHistory
   Add a new URL in the history associated with the window of document doc.
  ----------------------------------------------------------------------*/
void AddDocHistory (Document doc, char *url, char *initial_url,
		    char *form_data, ClickEvent method)
{
   int                 i, position, distance;

   if (!url || *url == EOS)
      return;
   /* avoid storing POST forms */
   if (method == CE_FORM_POST)
     return;
   /* don't register a new document not saved */
   if (!IsW3Path (url) && !TtaFileExist (url))
     return;
   else if (method == CE_RELATIVE || method == CE_HELP)
     /* All registered URLs are absolute */
     method = CE_ABSOLUTE;

   /* initialize the history if it has not been done yet */
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
     {
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	  {
	    DocHistory[doc][i].HistUrl = NULL;
	    DocHistory[doc][i].form_data = NULL;
	  }
	DocHistoryIndex[doc] = 0;
     }

   /* set the Back button on if necessary */
   i = DocHistoryIndex[doc];
   if (i ==  0)
      i = DOC_HISTORY_SIZE - 1;
   else
      i--;
   if (DocHistory[doc][i].HistUrl == NULL)
      /* there is no document before in the history */
      /* The Back button is normally OFF set it ON */
      SetArrowButton (doc, TRUE, TRUE);
     
   /* store the URL and the associated form data */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);
   
   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = TtaStrdup (url);
   DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl = TtaStrdup (initial_url);
   DocHistory[doc][DocHistoryIndex[doc]].form_data = TtaStrdup (form_data);
   DocHistory[doc][DocHistoryIndex[doc]].method = method;

   position = RelativePosition (doc, &distance);
   DocHistory[doc][DocHistoryIndex[doc]].HistDistance = distance;
   DocHistory[doc][DocHistoryIndex[doc]].HistPosition = position;

   DocHistoryIndex[doc]++;
   DocHistoryIndex[doc] %= DOC_HISTORY_SIZE; 

   /* delete the next entry in the history */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl);

   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);

   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].HistInitialUrl = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].form_data = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].method = CE_ABSOLUTE;

   /* set the Forward button off */
   SetArrowButton (doc, FALSE, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAmaya (Document document, View view)
{
   char                  localname[MAX_LENGTH];
#ifdef AMAYA_CRASH
   /* force amaya to crash : activate AMAYA_CRASH flag only for debug */
   memset(0,0,10);
#endif /* AMAYA_CRASH */
#ifdef AMAYA_DEBUG
   Element             el;
   View                structView, altView, linksView, tocView;
   int                 n;
   FILE               *list;

  /* get the root element */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "tree.debug");
   list = TtaWriteOpen (localname);
   el = TtaGetMainRoot (document);
   TtaListAbstractTree (el, list);
   TtaWriteClose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "view.debug");
   list = TtaWriteOpen (localname);
   TtaListView (document, 1, list);
   TtaWriteClose (list);
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "boxes.debug");
   list = TtaWriteOpen (localname);
   TtaListBoxes (document, 1, list);
   TtaWriteClose (list);
   structView = TtaGetViewFromName (document, "Structure_view");
   if (structView != 0 && TtaIsViewOpen (document, structView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "structview.debug");
       list = TtaWriteOpen (localname);
       TtaListView (document, structView, list);
       TtaWriteClose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "structboxes.debug");
       list = TtaWriteOpen (localname);
       TtaListBoxes (document, structView, list);
       TtaWriteClose (list);
     }
   altView = TtaGetViewFromName (document, "Alternate_view");
   if (altView != 0 && TtaIsViewOpen (document, altView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "altview.debug");
       list = TtaWriteOpen (localname);
       TtaListView (document, altView, list);
       TtaWriteClose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "altboxes.debug");
       list = TtaWriteOpen (localname);
       TtaListBoxes (document, altView, list);
       TtaWriteClose (list);
     }
   linksView = TtaGetViewFromName (document, "Links_view");
   if (linksView != 0 && TtaIsViewOpen (document, linksView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "linksview.debug");
       list = TtaWriteOpen (localname);
       TtaListView (document, linksView, list);
       TtaWriteClose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "linksboxes.debug");
       list = TtaWriteOpen (localname);
       TtaListBoxes (document, linksView, list);
       TtaWriteClose (list);
     }
   tocView = TtaGetViewFromName (document, "Table_of_contents");
   if (tocView != 0 && TtaIsViewOpen (document, tocView))
     {
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "tocview.debug");
       list = TtaWriteOpen (localname);
       TtaListView (document, tocView, list);
       TtaWriteClose (list);
       strcpy (localname, TempFileDirectory);
       strcat (localname, DIR_STR);
       strcat (localname, "tocboxes.debug");
       list = TtaWriteOpen (localname);
       TtaListBoxes (document, tocView, list);
       TtaWriteClose (list);
     }
   /* list now CSS rules */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "style.debug");
   list = TtaWriteOpen (localname);
   TtaListStyleSchemas (document, list);
   TtaWriteClose (list);
   /* list CSS rules applied to the current selection */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "style_element.debug");
   list = TtaWriteOpen (localname);
   n = TtaListStyleOfCurrentElement (document, list);
   if (n == 0)
     {
       fprintf (list, TtaGetMessage (AMAYA, AM_NO_STYLE_FOR_ELEM));
       fprintf (list, "\n");
     }
   TtaWriteClose (list);
   /* list now shortcuts */
   strcpy (localname, TempFileDirectory);
   strcat (localname, DIR_STR);
   strcat (localname, "shortcuts.debug");
   list = TtaWriteOpen (localname);
   TtaListShortcuts (document, list);
   TtaWriteClose (list);
#endif /* AMAYA_DEBUG */

#if defined(_GTK)
   TtaNewDialogSheet (BaseDialog + AboutForm, TtaGetViewFrame (document, view),
		      (char *)TtaGetAppName(), 1, TtaGetMessage(LIB, TMSG_LIB_CONFIRM), TRUE, 1,'L');
#endif  /* #if defined(_GTK) */
   
   strcpy (localname, TtaGetAppName());
   strcat (localname, " - ");
   strcat (localname, TtaGetAppVersion());
   strcat (localname, "     ");
   strcat (localname, TtaGetAppDate());
   
#if defined(_GTK)
   TtaNewLabel(BaseDialog + Version, BaseDialog + AboutForm, localname);
   TtaNewLabel(BaseDialog + About1, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT1));
   TtaNewLabel(BaseDialog + About2, BaseDialog + AboutForm,
	       TtaGetMessage(AMAYA, AM_ABOUT2));
   TtaShowDialogue (BaseDialog + AboutForm, FALSE);
#endif /* #if defined(_GTK) */

#ifdef _WINGUI
   CreateHelpDlgWindow (TtaGetViewFrame (document, view), localname,
			TtaGetMessage(AMAYA, AM_ABOUT1),
			TtaGetMessage(AMAYA, AM_ABOUT2));
#endif /* _WINGUI */

#ifdef _WX
   wxMessageDialog dlg( TtaGetViewFrame(document,view),
			TtaConvMessageToWX(localname)+_T("\n")+
			TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ABOUT1))+_T("\n")+
			TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ABOUT2)),
			_T(""), /* dialog title */
			wxOK | wxICON_INFORMATION | wxSTAY_ON_TOP,
			wxDefaultPosition );
   dlg.ShowModal();
#endif /* _WX */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAtW3C (Document document, View view)
{
  char      localname[MAX_LENGTH];

#ifdef LC
  TtaShowNamespaceDeclarations (document);
#endif /* LC */
  strcpy (localname, AMAYA_PAGE_DOC);
  strcat (localname, "BinDist.html");
#ifdef _WX
  LoadDefaultOpeningLocation();
  document = GetAmayaDoc (localname, NULL, document, document, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#else /* _WX */
  document = GetAmayaDoc (localname, NULL, 0, 0, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#endif /* _WX */
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpLocal (Document doc, View view)
{
  Document    document;
  char        localname[MAX_LENGTH];
  char       *s, *lang;

  lang = TtaGetVarLANG ();
  s = TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      /* get the welcome in the current language */
      sprintf (localname, "%s%camaya%c%s.%s", s, DIR_SEP, DIR_SEP,AMAYA_PAGE, lang);
      if (!TtaFileExist (localname))
	/* get the standard english documentation */
	sprintf (localname, "%s%camaya%c%s", s, DIR_SEP, DIR_SEP, AMAYA_PAGE);
    }
#ifdef _WX
  LoadDefaultOpeningLocation();
  document = GetAmayaDoc (localname, NULL, doc, doc, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#else /* _WX */
  document = GetAmayaDoc (localname, NULL, 0, 0, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#endif /* _WX */
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
static void DisplayHelp (int doc, int index)
{
  Document    document;
  char        localname[MAX_LENGTH];
  char       *s, *lang;
  char       *helpdir;

#ifdef _WX
  helpdir = "WX";
#else /* _WX */
  helpdir = "html";
#endif /* _WX */
  lang = TtaGetVarLANG ();
  s = TtaGetEnvString ("THOTDIR");
  if (s != NULL)
    {
      /* get the documentation in the current language */
      sprintf (localname, "%s%cdoc%c%s%c%s.%s", s, DIR_SEP, DIR_SEP,
		helpdir, DIR_SEP, Manual[index], lang);

      if (!TtaFileExist (localname))
      /* get the standard english documentation */
	sprintf (localname, "%s%cdoc%c%s%c%s", s, DIR_SEP, DIR_SEP,
		  helpdir, DIR_SEP, Manual[index]);
    }
#ifdef _WX
  LoadDefaultOpeningLocation();
  document = GetAmayaDoc (localname, NULL, doc, doc, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#else /* _WX */
  document = GetAmayaDoc (localname, NULL, 0, 0, (ClickEvent)CE_HELP,
			  FALSE, NULL, NULL);
#endif /* _WX */
  InitDocHistory (document);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpIndex (Document document, View view)
{
  DisplayHelp (document, INDEX);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpBrowsing (Document document, View view)
{
  DisplayHelp (document, BROWSING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSelecting (Document document, View view)
{
  DisplayHelp (document, SELECTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSearching (Document document, View view)
{
  DisplayHelp (document, SEARCHING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpViews (Document document, View view)
{
  DisplayHelp (document, VIEWS);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpDocument (Document document, View view)
{
  DisplayHelp (document, DOCUMENT);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpCreating (Document document, View view)
{
  DisplayHelp (document, CREATING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpLinks (Document document, View view)
{
  DisplayHelp (document, LINKS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpChanging (Document document, View view)
{
  DisplayHelp (document, CHANGING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpTables (Document document, View view)
{
  DisplayHelp (document, TABLES);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpMath (Document document, View view)
{
  DisplayHelp (document, MATH);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSVG (Document document, View view)
{
  DisplayHelp (document, SVG);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpEditChar (Document document, View view)
{
  DisplayHelp (document, EDITCHAR);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpXml (Document document, View view)
{
  DisplayHelp (document, XML);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpImageMaps (Document document, View view)
{
  DisplayHelp (document, IMAGEMAPS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpStyleSheets (Document document, View view)
{
  DisplayHelp (document, CSS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpAttributes (Document document, View view)
{
  DisplayHelp (document, ATTRIBUTES);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpSpellChecking (Document document, View view)
{
  DisplayHelp (document, SPELLCHECKING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpPublishing (Document document, View view)
{
  DisplayHelp (document, PUBLISHING);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpWebDAV (Document document, View view)
{
  DisplayHelp (document, WEBDAV);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpPrinting (Document document, View view)
{
  DisplayHelp (document, PRINTING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpNumbering (Document document, View view)
{
  DisplayHelp (document, NUMBERING);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpMakeBook (Document document, View view)
{
  DisplayHelp (document, MAKEBOOK);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpAnnotation (Document document, View view)
{
  DisplayHelp (document, ANNOTATE);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpBookmarks (Document document, View view)
{
  DisplayHelp (document, BOOK_MARKS);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpConfigure (Document document, View view)
{
  DisplayHelp (document, CONFIGURE);
}


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void HelpShortCuts (Document document, View view)
{
  DisplayHelp (document, SHORTCUTS);
}

/*----------------------------------------------------------------------
Accessibility help page. Added by Charles McCN oct 99
 -----------------------------------------------------------------------*/
void HelpAccess (Document document, View view)
{
  DisplayHelp (document, ACCESS);
}
