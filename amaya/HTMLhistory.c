/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#define EXPORT extern
#include "amaya.h"
#include "css.h"

#include "HTMLhistory_f.h"

char               *CSSHistory[CSS_HISTORY_SIZE];
char               *HTMLHistory[CSS_HISTORY_SIZE];
int                 CSSHistoryIndex = -1;
int                 HTMLHistoryIndex = -1;

#ifdef AMAYA_DEBUG
#define MSG(msg) fprintf(stderr,msg)
#else
static char        *last_message = NULL;

#define MSG(msg) last_message = msg
#endif

/*----------------------------------------------------------------------
   AddCSSHistory                                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddCSSHistory (CSSInfoPtr css)
#else  /* __STDC__ */
void                AddCSS (css)
CSSInfoPtr          css;

#endif /* __STDC__ */
{
   int                 i;

   if (!css)
      return;
   if (!css->url)
      return;
   if (css->category == CSS_DOCUMENT_STYLE)
      return;

   /* initialize the history if necessary */
   if (CSSHistoryIndex == -1)
     {
	for (i = 0; i < CSS_HISTORY_SIZE; i++)
	   CSSHistory[i] = NULL;
	CSSHistoryIndex = 0;
     }
   /* history lookup to store unique URL's */
   for (i = 0; i < CSS_HISTORY_SIZE; i++)
     {
	if (!CSSHistory[i])
	   break;
	if (strcmp (CSSHistory[i], css->url) != 0)
	   return;
     }
   /* store the CSS url */
   if (CSSHistory[CSSHistoryIndex])
      TtaFreeMemory (CSSHistory[CSSHistoryIndex]);
   CSSHistory[CSSHistoryIndex] = TtaStrdup (css->url);
   CSSHistoryIndex++;
   CSSHistoryIndex %= CSS_HISTORY_SIZE;

}

/*----------------------------------------------------------------------
   AddHTMLHistory                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddHTMLHistory (char *url)
#else  /* __STDC__ */
void                AddHTMLHistory (url)
char               *url;

#endif /* __STDC__ */
{
   int                 i;

   if (!url)
      return;

   /* initialize the history if necessary */
   if ((HTMLHistoryIndex < 0) || (HTMLHistoryIndex >= HTML_HISTORY_SIZE))
     {
	for (i = 0; i < HTML_HISTORY_SIZE; i++)
	   HTMLHistory[i] = NULL;
	HTMLHistoryIndex = HTML_HISTORY_SIZE - 1;
     }
   /* first check for reinstalling an existing URL */
   if ((HTMLHistory[HTMLHistoryIndex]) &&
       (!strcmp (HTMLHistory[HTMLHistoryIndex], url)))
      return;
   /* store the HTML url */
   HTMLHistoryIndex++;
   HTMLHistoryIndex %= HTML_HISTORY_SIZE;
   if (HTMLHistory[HTMLHistoryIndex])
      TtaFreeMemory (HTMLHistory[HTMLHistoryIndex]);
   HTMLHistory[HTMLHistoryIndex] = TtaStrdup (url);
}

/*----------------------------------------------------------------------
   BuildCSSHistoryList : Build the whole list of CSS in the history  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 BuildCSSHistoryList (Document doc, char *buf, int size, char *first)
#else  /* __STDC__ */
int                 BuildCSSHistoryList (doc, buf, size, first)
Document            doc;
char               *buf;
int                 size;
char               *first;

#endif /* __STDC__ */
{
   int                 free = size;
   int                 len;
   int                 nb = 0;
   int                 index = 0;
   int                 i;
   char               *url;

   /*
    * ad the first element if specified.
    */
   buf[0] = 0;
   if (first)
     {
	strcpy (&buf[index], first);
	len = strlen (first);
	len++;
	free -= len;
	index += len;
	nb++;
     }

   for (i = 0; i < CSS_HISTORY_SIZE; i++)
     {
	url = CSSHistory[i];
	if (!url)
	   break;
	len = strlen (url);
	len++;
	if (len >= free)
	  {
	     MSG ("BuildCSSHistoryList : Too many styles\n");
	     break;
	  }
	strcpy (&buf[index], url);
	free -= len;
	index += len;
	nb++;
     }

#ifdef DEBUG_CSS
   fprintf (stderr, "BuildCSSHistoryList : found %d CSS\n", nb);
#endif
   return (nb);
}
