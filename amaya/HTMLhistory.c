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

#define DOC_HISTORY_SIZE 32

/* a record in an history */
typedef struct _HistElement
{
	char*	HistUrl;	/* document URL */
	int	HistPosition;	/* volume preceding the first element to be
				made visible in the main window */
	int	HistDistance;	/* distance from the  top of the window to the
				top of this element (% of the window height) */
} HistElement;

/* the history of a window */
typedef HistElement   anHistory[DOC_HISTORY_SIZE];

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
   DocHistoryIndex[(int) doc] = -1;
}

/*----------------------------------------------------------------------
  ElementAtPosition
  Returns the element that is at position pos in document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element	ElementAtPosition (Document doc, int pos)
#else  /* __STDC__ */
static Element	ElementAtPosition (doc, pos)
   Document	doc;
   int		pos;
#endif /* __STDC__ */
{
   Element	el, result, child, next;
   int		sum, vol;
   boolean	stop;

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
static int      RelativePosition (Document doc, int *distance)
#else  /* __STDC__ */
static int      RelativePosition (doc, distance)
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


/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _GotoHistory_context {
  Document             doc;
  int                  prevnext;
  boolean	       last;
} GotoHistory_context;

/*----------------------------------------------------------------------
   GotoPreviousHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               GotoPreviousHTML_callback (int newdoc, int status, 
					      char *urlName,
					      char *outputfile, 
					      char *content_type,
					      void * context)
#else  /* __STDC__ */
void               GotoPreviousHTML_callback (newdoc, status, urlName,
                                             outputfile, content_type, 
                                             context)
int newdoc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;

#endif
{
  Document             doc;
  Element	       el;
  int                  prev;
  boolean	       last;
  GotoHistory_context *ctx = (GotoHistory_context *) context;

  if (ctx == NULL)
    return;

  prev = ctx->prevnext;
  last = ctx->last;
  doc = ctx->doc;

   /* show the document at the position stored in the history */
   el = ElementAtPosition (doc, DocHistory[doc][prev].HistPosition);
   TtaShowElement (doc, 1, el, DocHistory[doc][prev].HistDistance);

   DocHistoryIndex[doc] = prev;

   /* set the Forward button on if it was the last document in the history */
   if (last)
      SetArrowButton (doc, FALSE, TRUE);

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
   int                 prev, i;
   char               *url = NULL;
   boolean	       last, hist;
   GotoHistory_context *ctx;

   if (DocHistoryIndex[doc] < 0)
      return;
   if (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
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

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (strcmp(DocumentURLs[doc], DocHistory[doc][prev].HistUrl))
      if (!CanReplaceCurrentDocument (doc, view))
         return;

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
      /* there is no previous document */
      /* set the Back button OFF */
      SetArrowButton (doc, TRUE, FALSE);

   /* load the previous document */
   url = DocHistory[doc][prev].HistUrl;

   /* save the context */
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = prev;
   ctx->last = last;
   ctx->doc = doc;
      
   /* is it the current document ? */
   if (!strcmp (url, DocumentURLs[doc]))
     {
       /* it's just a move in the same document */
       if (hist)
	 /* record the current position in the history */
	 AddDocHistory (doc, url);
       GotoPreviousHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
     }
   else
     (void) GetHTMLDocument (url, NULL, doc, doc, CE_FALSE, hist, (void *) GotoPreviousHTML_callback, (void *) ctx);
}

/*----------------------------------------------------------------------
   GotoNextHTML_callback
   This function is called when the document is loaded
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               GotoNextHTML_callback (int newdoc, int status, 
					      char *urlName,
					      char *outputfile, 
					      char *content_type,
					      void * context)
#else  /* __STDC__ */
void               GotoNextHTML_callback (newdoc, status, urlName,
                                             outputfile, content_type, 
                                             context)
int newdoc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;

#endif
{
  Element	       el;
  int                 next;
  Document             doc;
  GotoHistory_context     *ctx = (GotoHistory_context *) context;
  
  /* retrieve the context */

  if (ctx == NULL)
    return;

  next = ctx->prevnext;
  doc = ctx->doc;

   /* show the document at the position stored in the history */
   el = ElementAtPosition (doc, DocHistory[doc][next].HistPosition);
   TtaShowElement (doc, 1, el, DocHistory[doc][next].HistDistance);

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
   char         *url = NULL;
   int		next, i;
   GotoHistory_context  *ctx;

   if (DocHistoryIndex[doc] < 0)
      return;
   if (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
      return;

   /* next entry in history */
   next = DocHistoryIndex[doc] + 1;
   next %= DOC_HISTORY_SIZE;

   /* nothing to do if there is no next document */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl == NULL)
      return;
   if (DocHistory[doc][next].HistUrl == NULL)
      return;

   /* if the document has been edited, ask the user to confirm, except
      if it's simply a jump in the same document */
   if (strcmp(DocumentURLs[doc], DocHistory[doc][next].HistUrl))
      if (!CanReplaceCurrentDocument (doc, view))
         return;

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

   /* load the next document */
   DocHistoryIndex[doc] = next;
   url = DocHistory[doc][next].HistUrl;

   /* save the context */
   ctx = TtaGetMemory (sizeof (GotoHistory_context));
   ctx->prevnext = next;
   ctx->doc = doc;

   /* is it the current document ? */
   if (!strcmp (url, DocumentURLs[doc]))
     /* it's just a move in the same document */
     GotoNextHTML_callback (doc, 0, url, NULL, NULL, (void *) ctx);
   else
     (void) GetHTMLDocument (url, NULL, doc, doc, CE_FALSE, FALSE, (void *) GotoNextHTML_callback, (void *) ctx);
}

/*----------------------------------------------------------------------
   AddDocHistory
   Add a new URL in the history associated with the window of document doc.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddDocHistory (Document doc, char *url)
#else  /* __STDC__ */
void                AddDocHistory (doc, url)
Document	    doc;
char               *url;

#endif /* __STDC__ */
{
   int                 i, position, distance;

   if (!url)
      return;
   if (*url == '\0')
      return;

   /* initialize the history if it has not been done yet */
   if ((DocHistoryIndex[doc] < 0) || (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE))
     {
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	   DocHistory[doc][i].HistUrl = NULL;
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

   /* store the URL */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
      TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = TtaStrdup (url);

   position = RelativePosition (doc, &distance);
   DocHistory[doc][DocHistoryIndex[doc]].HistDistance = distance;
   DocHistory[doc][DocHistoryIndex[doc]].HistPosition = position;

   DocHistoryIndex[doc]++;
   DocHistoryIndex[doc] %= DOC_HISTORY_SIZE;

   /* delete the next entry in the history */
   if (DocHistory[doc][DocHistoryIndex[doc]].HistUrl)
      TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]].HistUrl);
   DocHistory[doc][DocHistoryIndex[doc]].HistUrl = NULL;

   /* set the Forward button off */
   SetArrowButton (doc, FALSE, FALSE);
}
