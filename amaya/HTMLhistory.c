/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: D. Veillard
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "HTMLhistory_f.h"
#include "init_f.h"

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
   IsLoaded                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          IsLoaded (char *url)
#else
static int          IsLoaded (url)
char               *url;

#endif
{
   int                 d;

   if (url == NULL)
      return (TRUE);
   for (d = 0; d < DocumentTableLength; d++)
      if (DocumentURLs[d] != NULL)
	 if (!strcmp (DocumentURLs[d], url))
	    return (TRUE);
   return (FALSE);
}

/*----------------------------------------------------------------------
   GotoPreviousHTML                                               
  ----------------------------------------------------------------------*/
/*ARGUSED */
#ifdef __STDC__
void                GotoPreviousHTML (Document doc, View view)
#else
void                GotoPreviousHTML (doc, view)
Document            document;
View                view;

#endif
{
   int                 i;
   char               *url = NULL;
   int                 base = HTMLHistoryIndex;

   if (HTMLHistoryIndex < 0)
      return;
   if (HTMLHistoryIndex >= HTML_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
      return;

   do
     {
	if (HTMLHistoryIndex == 0)
	  {
	     for (i = HTML_HISTORY_SIZE - 1; i >= 0; i--)
	       {
		  if (HTMLHistory[i] != NULL)
		    {
		       HTMLHistoryIndex = i;
		       url = HTMLHistory[i];
		       break;
		    }
	       }
	  }
	else
	  {
	     HTMLHistoryIndex--;
	     url = HTMLHistory[HTMLHistoryIndex];
	  }
	if (!IsLoaded (url))
	   break;
	else
	   url = NULL;
     }
   while (base != HTMLHistoryIndex);
   if (url != NULL)

      (void) GetHTMLDocument (url, NULL, doc, doc, DC_FALSE);
}

/*----------------------------------------------------------------------
   GotoNextHTML                                                   
  ----------------------------------------------------------------------*/
/*ARGUSED */
#ifdef __STDC__
void                GotoNextHTML (Document doc, View view)
#else
void                GotoNextHTML (doc, view)
Document            document;
View                view;

#endif
{
   char               *url = NULL;
   int                 base = HTMLHistoryIndex;

   if (HTMLHistoryIndex < 0)
      return;
   if (HTMLHistoryIndex >= HTML_HISTORY_SIZE)
      return;
   if ((doc < 0) || (doc >= DocumentTableLength))
      return;

   do
     {
	url = HTMLHistory[HTMLHistoryIndex];
	HTMLHistoryIndex++;
	HTMLHistoryIndex %= HTML_HISTORY_SIZE;
	if (!IsLoaded (url))
	   break;
	else
	   url = NULL;
     }
   while (base != HTMLHistoryIndex);
   if (url != NULL)
      (void) GetHTMLDocument (url, NULL, doc, doc, DC_FALSE);
}

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
