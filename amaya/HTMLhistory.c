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

   if (DocHistoryIndex[doc] < 0)
      return;
   if (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
      return;
   if (!CanReplaceCurrentDocument (doc, view))
      return;

   /* previous document in history */
   prev = DocHistoryIndex[doc];
   if (prev ==  0)
      prev = DOC_HISTORY_SIZE - 1;
   else
      prev--;

   /* nothing to do if there is no previous document */
   if (DocHistory[doc][prev] == NULL)
      return;

   /* set the Forward button on if the next document is the last one
      in the history */
   i = DocHistoryIndex[doc];
   i++;
   i %= DOC_HISTORY_SIZE;
   if (DocHistory[doc][i] == NULL)
      SetArrowButton (doc, FALSE, TRUE);

   /* set the Back button off if there is no document before the previous one*/
   i = prev;
   if (i ==  0)
      i = DOC_HISTORY_SIZE - 1;
   else
      i--;
   if (DocHistory[doc][i] == NULL)
      /* there is no document before the previous one */
      /* set the Back button OFF */
      SetArrowButton (doc, TRUE, FALSE);

   /* load the previous document */
   DocHistoryIndex[doc] = prev;
   url = DocHistory[doc][prev];
   (void) GetHTMLDocument (url, NULL, doc, doc, DC_FALSE, FALSE);
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
   char               *url = NULL;
   int		       next, i;

   if (DocHistoryIndex[doc] < 0)
      return;
   if (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
      return;
   if (!CanReplaceCurrentDocument (doc, view))
      return;

   /* next entry in history */
   next = DocHistoryIndex[doc] + 1;
   next %= DOC_HISTORY_SIZE;

   /* nothing to do if there is no next document */
   if (DocHistory[doc][next] == NULL)
      return;

   /* set the Back button on if it's off */
   i = DocHistoryIndex[doc];
   if (i ==  0)
      i = DOC_HISTORY_SIZE - 1;
   else
      i--;
   if (DocHistory[doc][i] == NULL)
      /* there is no document before the current one. The Back button is
         normally OFF */
      /* set the Back button ON */
      SetArrowButton (doc, TRUE, TRUE);

   /* set the Forward button off if the next document is the last one
      in the history */
   i = next;
   i++;
   i %= DOC_HISTORY_SIZE;
   if (DocHistory[doc][i] == NULL)
      SetArrowButton (doc, FALSE, FALSE);

   /* load the next document */
   DocHistoryIndex[doc] = next;
   url = DocHistory[doc][DocHistoryIndex[doc]];
   (void) GetHTMLDocument (url, NULL, doc, doc, DC_FALSE, FALSE);
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
   int                 i;

   if (!url)
      return;
   if (*url == '\0')
      return;

   /* initialize the history if necessary */
   if ((DocHistoryIndex[doc] < 0) || (DocHistoryIndex[doc] >= DOC_HISTORY_SIZE))
     {
	for (i = 0; i < DOC_HISTORY_SIZE; i++)
	   DocHistory[doc][i] = NULL;
	DocHistoryIndex[doc] = DOC_HISTORY_SIZE - 1;
     }
   /* first check for reinstalling an existing URL */
   if ((DocHistory[doc][DocHistoryIndex[doc]]) &&
       (!strcmp (DocHistory[doc][DocHistoryIndex[doc]], url)))
      return;

   /* set the Back button on if necessary */
   if (DocHistory[doc][DocHistoryIndex[doc]] != NULL)
      /* there is a previous document */
      {
      i = DocHistoryIndex[doc];
      if (i ==  0)
         i = DOC_HISTORY_SIZE - 1;
      else
         i--;
      if (DocHistory[doc][i] == NULL)
	 /* there is no document before the previous one */
	 /* The Back button is normally OFF */
	 /* set the Back button ON */
         SetArrowButton (doc, TRUE, TRUE);
      }

   /* store the URL */
   DocHistoryIndex[doc]++;
   DocHistoryIndex[doc] %= DOC_HISTORY_SIZE;
   if (DocHistory[doc][DocHistoryIndex[doc]])
      TtaFreeMemory (DocHistory[doc][DocHistoryIndex[doc]]);
   DocHistory[doc][DocHistoryIndex[doc]] = TtaStrdup (url);

   /* delete the next entry in the history */
   i = DocHistoryIndex[doc];
   i++;
   i %= DOC_HISTORY_SIZE;
   if (DocHistory[doc][i])
      TtaFreeMemory (DocHistory[doc][i]);
   DocHistory[doc][i] = NULL;

   /* set the Forward button off */
   SetArrowButton (doc, FALSE, FALSE);
}
