/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"

#include "HTMLhistory_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"

#define DOC_HISTORY_SIZE 32

/* a record in an history */
typedef struct _HistElement
{
     STRING HistUrl;        /* document URL */
     STRING form_data;      /* data associated with forms */
     int    method;         /* method used to request this URL */
     int    HistPosition;	/* volume preceding the first element to be
                               made visible in the main window */
     int    HistDistance;	/* distance from the  top of the window to the
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
} GotoHistory_context;

/* the history of all windows */
static anHistory    DocHistory[DocumentTableLength];
/* current position in the history of each window */
static int          DocHistoryIndex[DocumentTableLength];

/*----------------------------------------------------------------------
   InitDocHistory
   Reset history for document doc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDocHistory (Document doc)
#else
void                InitDocHistory (doc)
Document            document;

#endif
{
   DocHistoryIndex[doc] = -1;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                FreeDocHistory ()
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
#ifdef __STDC__
Element       	ElementAtPosition (Document doc, int pos)
#else  /* __STDC__ */
Element       	ElementAtPosition (doc, pos)
   Document	doc;
   int		pos;
#endif /* __STDC__ */
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
#ifdef __STDC__
int             RelativePosition (Document doc, int *distance)
#else  /* __STDC__ */
int             RelativePosition (doc, distance)
   Document	doc;
   int		*distance;
#endif /* __STDC__ */
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
#ifdef __STDC__
static ThotBool IsNextDocLoaded (const Document baseDoc, const STRING url, const STRING form_data, const ClickEvent CE_event)
#else
static ThotBool IsNextDocLoaded (Document baseDoc, url, form_data, CE_event)
STRING url;
STRING form_data;
ClickEvent  CE_event;
#endif
{
  STRING              tempdocument;
  STRING              target;
  STRING              documentname;
  STRING              parameters;
  STRING              pathname;
  ThotBool loaded;

  if (url == (STRING) NULL)
    return FALSE;

  tempdocument = TtaAllocString (MAX_LENGTH);
  target = TtaAllocString (MAX_LENGTH);
  documentname = TtaAllocString (MAX_LENGTH);
  parameters = TtaAllocString (MAX_LENGTH);
  pathname = TtaAllocString (MAX_LENGTH);

  ustrcpy (tempdocument, url);
  ExtractParameters (tempdocument, parameters);
  /* Extract the target if necessary */
  ExtractTarget (tempdocument, target);

  /* Add the  base content if necessary */
  if (CE_event == CE_RELATIVE || CE_event == CE_FORM_GET
      || CE_event == CE_FORM_POST || CE_event == CE_MAKEBOOK)
    NormalizeURL (tempdocument, baseDoc, pathname, documentname, NULL);
  else
    NormalizeURL (tempdocument, 0, pathname, documentname, NULL);

  /* if it's a file: url, we remove the protocol, as it
     is a local file */
  ConvertFileURL (pathname);
  
  loaded = IsDocumentLoaded (pathname, form_data);

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
#ifdef __STDC__
void               GotoPreviousHTML_callback (int newdoc, int status, 
					      STRING urlName,
					      STRING outputfile, 
					      AHTHeaders *http_headers,
					      void * context)
#else  /* __STDC__ */
void               GotoPreviousHTML_callback (newdoc, status, urlName,
                                             outputfile, http_headers,
                                             context)
int newdoc;
int status;
STRING urlName;
STRING outputfile;
AHTHeaders *http_headers;
void *context;
#endif
{
  Document             doc;
  Element	       el;
  GotoHistory_context *ctx = (GotoHistory_context *) context;
  int                  prev;

  if (ctx == NULL)
    return;

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
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
   GotoPreviousHTML
   This function is called when the user presses the Previous button
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GotoPreviousHTML (Document doc, View view)
#else
void                GotoPreviousHTML (doc, view)
Document            document;
View                view;
#endif
{
   GotoHistory_context *ctx;
   STRING              url = NULL;
   STRING              form_data = NULL;
   int                 prev, i;
   int                 method;
   ThotBool	       last, hist;
   ThotBool            same_form_data;
   ThotBool            next_doc_loaded = FALSE;

   if (doc < 0 || doc >= DocumentTableLength)
      return;
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;

   /* previous document in history */
   prev = DocHistoryIndex[doc];
   if (prev ==  0)
      prev = DOC_HISTORY_SIZE - 1;
   else
      prev--;
 
   /* nothing to do if there is no previous document */
   if (DocHistory[doc][prev].HistUrl == NULL)
      return;

   /* get the previous document information*/
   url = DocHistory[doc][prev].HistUrl;
   form_data = DocHistory[doc][prev].form_data;
   method = DocHistory[doc][prev].method;

   /* is it the same form_data? */
   if (!form_data && (!DocumentMeta[doc]  || !DocumentMeta[doc]->form_data))
     same_form_data = TRUE;
   else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	    && (!ustrcmp (form_data, DocumentMeta[doc]->form_data)))
     same_form_data = TRUE;
   else
     same_form_data = FALSE;

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (DocumentURLs[doc] != NULL)
     {
       if (ustrcmp(DocumentURLs[doc], url)
	   || !same_form_data)
	 {
	   /* is the next document already loaded? */
	   next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
	   if (!next_doc_loaded && !CanReplaceCurrentDocument (doc, view))
	     return;
	 }
     }


   if (!next_doc_loaded)
     {
       /* the current document must be put in the history if it's the last one */
       hist = FALSE;
       last = FALSE;
       if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
	 {
	   last = TRUE;
	   hist = TRUE;
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
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = prev;
   ctx->last = last;
   ctx->doc = doc;
   ctx->next_doc_loaded = next_doc_loaded;

   /* 
   ** load (or jump to) the previous document 
   */
   if (!next_doc_loaded)
     {
     if (hist)
       /* record the current position in the history */
       AddDocHistory (doc, DocumentURLs[doc], DocumentMeta[doc]->form_data, DocumentMeta[doc]->method);
     
     DocHistoryIndex[doc] = prev;
     }

   /* is it the current document ? */     
   if (DocumentURLs[doc] && !ustrcmp (url, DocumentURLs[doc]) && same_form_data)
     {
       /* it's just a move in the same document */
       GotoPreviousHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
     }
   else
     {
       StopTransfer (doc, 1);
       (void) GetHTMLDocument (url, form_data, doc, doc, method, FALSE, (void *) GotoPreviousHTML_callback, (void *) ctx);
     }
}

/*----------------------------------------------------------------------
   GotoNextHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               GotoNextHTML_callback (int newdoc, int status, 
					      STRING urlName,
					      STRING outputfile, 
					      AHTHeaders *http_headers,
					      void * context)
#else  /* __STDC__ */
void               GotoNextHTML_callback (newdoc, status, urlName,
                                             outputfile, http_headers,
                                             context)
int newdoc;
int status;
STRING urlName;
STRING outputfile;
AHTHeaders *http_headers;
void *context;
#endif
{
  Element	       el;
  Document             doc;
  GotoHistory_context     *ctx = (GotoHistory_context *) context;
  int                  next;
  
  /* retrieve the context */

  if (ctx == NULL)
    return;

  next = ctx->prevnext;
  doc = ctx->doc;
  if (doc == newdoc && DocHistoryIndex[doc] == next)
    {
      /* we are still working on the same document */
      /* show the document at the position stored in the history */
      el = ElementAtPosition (doc, DocHistory[doc][next].HistPosition);
      TtaShowElement (doc, 1, el, DocHistory[doc][next].HistDistance);
    }
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
   GotoNextHTML
   This function is called when the user presses the Next button
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GotoNextHTML (Document doc, View view)
#else
void                GotoNextHTML (doc, view)
Document            document;
View                view;
#endif
{
   GotoHistory_context  *ctx;
   STRING        url = NULL;
   STRING        form_data = NULL;
   int           method;
   int		 next, i;
   ThotBool      same_form_data;
   ThotBool      next_doc_loaded = FALSE;

   if (doc < 0 || doc >= DocumentTableLength)
      return;
   if (DocHistoryIndex[doc] < 0 || DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;

   /* next entry in history */
   next = DocHistoryIndex[doc] + 1;
   next %= DOC_HISTORY_SIZE;

   /* nothing to do if there is no next document */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
      return;
   if (DocHistory[doc][next].HistUrl == NULL)
      return;

   /* Get the next document information */
   url = DocHistory[doc][next].HistUrl;
   form_data = DocHistory[doc][next].form_data;
   method = DocHistory[doc][next].method;

   /* is the form_data the same? */
   if (!form_data && (!DocumentMeta[doc] || !DocumentMeta[doc]->form_data))
     same_form_data = TRUE;
   else if (form_data && DocumentMeta[doc] && DocumentMeta[doc]->form_data 
	    && (!ustrcmp (form_data, DocumentMeta[doc]->form_data)))
     same_form_data = TRUE;
   else
     same_form_data = FALSE;

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (DocumentURLs[doc] != NULL)
     {
       if (ustrcmp (DocumentURLs[doc], DocHistory[doc][next].HistUrl)
	   || !same_form_data)
	 {
	   /* is the next document already loaded? */
	   next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
	   if (!CanReplaceCurrentDocument (doc, view))
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
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = next;
   ctx->doc = doc;
   ctx->next_doc_loaded = next_doc_loaded;

   /* is it the current document ? */
   if (DocumentURLs[doc] && !ustrcmp (url, DocumentURLs[doc]) && same_form_data)
     /* it's just a move in the same document */
     GotoNextHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
   else
     {
       StopTransfer (doc, 1);
       (void) GetHTMLDocument (url, form_data, doc, doc, method, FALSE, (void *) GotoNextHTML_callback, (void *) ctx);
     }
}

/*----------------------------------------------------------------------
   AddDocHistory
   Add a new URL in the history associated with the window of document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddDocHistory (Document doc, STRING url, STRING form_data, ClickEvent method)
#else  /* __STDC__ */
void                AddDocHistory (doc, url, form_data, method)
Document	    doc;
STRING              url;
STRING              form_data;
ClickEvent          method;

#endif /* __STDC__ */
{
   int                 i, position, distance;

   if (!url)
      return;
   if (*url == EOS)
      return;
   /* avoid storing POST forms */
   if (method == CE_FORM_POST)
     return;
   else if (method == CE_RELATIVE)
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
   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
     TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);
   
   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = TtaWCSdup (url);
   DocHistory[doc][DocHistoryIndex[doc]].form_data = TtaWCSdup (form_data);
   DocHistory[doc][DocHistoryIndex[doc]].method = method;

   position = RelativePosition (doc, &distance);
   DocHistory[doc][DocHistoryIndex[doc]].HistDistance = distance;
   DocHistory[doc][DocHistoryIndex[doc]].HistPosition = position;

   DocHistoryIndex[doc]++;
   DocHistoryIndex[doc] %= DOC_HISTORY_SIZE; 

   /* delete the next entry in the history */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   if (DocHistory[doc][DocHistoryIndex[doc]].form_data)
       TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].form_data);

   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].form_data = NULL;
   DocHistory[doc][DocHistoryIndex[doc]].method = CE_ABSOLUTE;

   /* set the Forward button off */
   SetArrowButton (doc, FALSE, FALSE);
}
