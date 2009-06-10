/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: V. Quint, J. Kahan, I. Vatton, L. Carcone
 *
 */

#include "wx/wx.h"
 
#define THOT_EXPORT extern
#include "amaya.h"
#include "helpmenu.h"
#include "css.h"
#include "MENUconf.h"
#include "tips.h"

#include "AHTURLTools_f.h"
#include "HTMLhistory_f.h"
#include "HTMLsave_f.h"
#include "init_f.h"
#include "MENUconf_f.h"

#include "wx/msgdlg.h"
#include "wx/aboutdlg.h"
#include "wx/ffile.h"
#include "message_wx.h"
#include "registry_wx.h"

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

/* Table of passwords */
#define MAX_PM_TABLE   50
static char     *PM_Server[MAX_PM_TABLE];
static char     *PM_Realm[MAX_PM_TABLE];
static char     *PM_Name[MAX_PM_TABLE];
static char     *PM_Passwd[MAX_PM_TABLE];
static int PM_Index = 0;
static int PM_Save = FALSE;

#define S_FILE "2906642345.s"
static Prop_Passwords_Site* Pass_FirstSite = NULL;

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
                                 const char *form_data, const int CE_event)
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
  HasPreviousDoc
  This function returns TRUE if there is a previous document
  ----------------------------------------------------------------------*/
ThotBool HasPreviousDoc (Document doc)
{
  int prev;

  if (doc == 0)
    return FALSE;
  /* check if the request comes from the source document */
  if (doc && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  prev = DocHistoryIndex[doc];
  if (prev ==  0)
    prev = DOC_HISTORY_SIZE - 1;
  else
    prev--;
  return (DocHistory[doc][prev].HistUrl != NULL);
}

/*----------------------------------------------------------------------
  HasNextDoc
  This function returns TRUE if there is a next document
  ----------------------------------------------------------------------*/
ThotBool HasNextDoc (Document doc)
{
  int next;

  if (doc == 0)
    return FALSE;
  /* check if the request comes from the source document */
  if (doc && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);
  next = DocHistoryIndex[doc] + 1;
  next %= DOC_HISTORY_SIZE;
  return (DocHistory[doc][next].HistUrl != NULL);
}

/*----------------------------------------------------------------------
  GotoPreviousHTML_callback
  This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoPreviousHTML_callback (int newdoc, int status, char *urlName, char *outputfile,
                                char *proxyName, AHTHeaders *http_headers, void * context)
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
  if (DocumentMeta[doc])
    {
      if (!DocumentMeta[doc]->initial_url)
        DocumentMeta[doc]->initial_url = ctx->initial_url;
      else
        TtaFreeMemory (ctx->initial_url);
    }
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

  /* check if the request comes from the source document */
  if (doc && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);

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
      next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
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
    GotoPreviousHTML_callback (doc, 0, url, NULL, NULL, NULL, (void *) ctx);
  else
    {
      StopTransfer (doc, 1);
      GetAmayaDoc (url, form_data, doc, doc, method, FALSE,
                   (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))
                   GotoPreviousHTML_callback,(void *) ctx);
      /* out of the critic section */
      Back_Forward = FALSE;
    }
}

/*----------------------------------------------------------------------
  GotoNextHTML_callback
  This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void GotoNextHTML_callback (int newdoc, int status, char *urlName, char *outputfile,
                            char *proxyName, AHTHeaders *http_headers, void * context)
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
  if (DocumentMeta[doc])
    {  if (!DocumentMeta[doc]->initial_url)
      DocumentMeta[doc]->initial_url = ctx->initial_url;
    else
      TtaFreeMemory (ctx->initial_url);
    }
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
  int		        next, i;
  ThotBool              same_form_data;
  ThotBool              next_doc_loaded = FALSE;

  /* check if the request comes from the source document */
  if (doc && DocumentTypes[doc] == docSource)
    doc = GetDocFromSource (doc);

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
      next_doc_loaded = IsNextDocLoaded (doc, url, form_data, method);
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
    GotoNextHTML_callback (doc, 0, url, NULL, NULL, NULL, (void *) ctx);
  else
    {
      StopTransfer (doc, 1);
      GetAmayaDoc (url, form_data, doc, doc, method, FALSE,
                   (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) GotoNextHTML_callback, (void *) ctx);
      /* out of the critic section */
      Back_Forward = FALSE;
    }
}

/*----------------------------------------------------------------------
  AddDocHistory
  Add a new URL in the history associated with the window of document doc.
  ----------------------------------------------------------------------*/
void AddDocHistory (Document doc, char *url, char *initial_url,
                    char *form_data, int method)
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


#ifdef AMAYA_DEBUG
void TtaDumpDocumentReference();
void DumpTemplateReferences ();
#endif /* AMAYA_DEBUG */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAmaya (Document document, View view)
{
  wxString str;
  wxAboutDialogInfo info;

#if !defined(_WINDOWS) && !defined(_MACOS)
  char *s = TtaGetEnvString ("THOTDIR");
#endif /* _WINDOWS && _MACOS */
  
#ifdef AMAYA_CRASH
  /* force amaya to crash : activate AMAYA_CRASH flag only for debug */
  memset(0, 0, 10);
#endif /* AMAYA_CRASH */
#ifdef AMAYA_DEBUG
  char localname[MAX_LENGTH];
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

  wxIcon icon(TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "logo.png"), wxBITMAP_TYPE_PNG);
  info.SetName(TtaConvMessageToWX(TtaGetAppName()));
  info.SetVersion(TtaConvMessageToWX(TtaGetAppVersion()) + wxT(" (") + TtaConvMessageToWX(TtaGetAppDate()) + wxT(")"));
  info.SetDescription(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ABOUT1)));
  str = TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ABOUT_COPYRIGHT));
  str.Replace(wxT("%s"), TtaConvMessageToWX(TtaGetAppYear()));
  info.SetCopyright(str);
  // Dont work :
  //  info.SetWebSite(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ABOUT_WEBSITE)));
  info.SetIcon(icon);
  
#if !defined(_WINDOWS) && !defined(_MACOS)
  if (s != NULL)
    {
      char welcomename[MAX_LENGTH];
      /* get the welcome in the current language */
      sprintf (welcomename, "%s%camaya%cCOPYRIGHT", s, DIR_SEP, DIR_SEP);      
      wxFFile file(TtaConvMessageToWX(welcomename));
      wxString str;
      if (file.ReadAll(&str, wxConvISO8859_1))
        {
          info.SetLicence(str);
        }
    }
#endif /* _WINDOWS && MACOS */
  wxAboutBox(info);

#ifdef AMAYA_DEBUG
  TtaDumpDocumentReference();
  DumpTemplateReferences();
#endif /* AMAYA_DEBUG */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpAtW3C (Document document, View view)
{
  char      localname[MAX_LENGTH];

  strcpy (localname, AMAYA_PAGE_DOC);
  strcat (localname, "BinDist.html");
  LoadDefaultOpeningLocation (TRUE); //in new frame
  document = GetAmayaDoc (localname, NULL, document, document,
                          CE_HELP,
                          FALSE, NULL, NULL);
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HelpLocal (Document doc, View view)
{
  Document    document;
  Element     root;
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
  LoadDefaultOpeningLocation (TRUE); // in new frame
  document = GetAmayaDoc (localname, NULL, doc, doc, CE_HELP,
                          FALSE, NULL, NULL);
  /* Set the Help document in ReadOnly mode */
  root = TtaGetMainRoot (document);
  TtaSetAccessRight (root, ReadOnly, document);
  InitDocHistory (document);
}


/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static void DisplayHelp (int doc, int index)
{
  Document    document;
  Element     root;
  char        localname[MAX_LENGTH];
  char       *s, *lang;
  const char *helpdir = "WX";

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
  LoadDefaultOpeningLocation (TRUE); //in new frame
  document = GetAmayaDoc (localname, NULL, doc, doc, CE_HELP,
                          FALSE, NULL, NULL);
  /* Set the Help document in ReadOnly mode */
  root = TtaGetMainRoot (document);
  TtaSetAccessRight (root, ReadOnly, document);

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
void HelpTemplating (Document document, View view)
{
  DisplayHelp (document, TEMPLATING);
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

/*----------------------------------------------------------------------
  Typ of the day
  -----------------------------------------------------------------------*/
void TipOfTheDay (Document document, View view)
{
  TtaShowTipOfTheDay();
}





/*----------------------  Password Manager -----------------------------*/

/*----------------------------------------------------------------------
  LoadPasswordTable
  Initialize the table of passwords
  ----------------------------------------------------------------------*/
static void LoadPasswordTable ()
{
  unsigned char   c;
  char            pathname[MAX_LENGTH];
  char            line[MAX_TXT_LEN];
  FILE           *file;
  int             len;
  ThotBool        endfile, ok;
  char            inbuf[MAX_LENGTH], outbuf[MAX_LENGTH];
  char            server[MAX_LENGTH], realm[MAX_LENGTH];
  char            name[MAX_LENGTH], passwd[MAX_LENGTH];


  /* Initialization */
  memset (PM_Server, 0, MAX_PM_TABLE);
  memset (PM_Realm, 0, MAX_PM_TABLE);
  memset (PM_Name, 0, MAX_PM_TABLE);
  memset (PM_Passwd, 0, MAX_PM_TABLE);
  PM_Save = FALSE;
  PM_Index = 0;

  /* Read the password file and store the records */
  /* Open the password file */
  file = NULL;
  sprintf (pathname, "%s%c%s", TempFileDirectory, DIR_SEP, S_FILE);
  file = TtaReadOpen (pathname);
  if (file == NULL)
    {
      return;
    }

  endfile = FALSE;
  ok  = TRUE;
  while (!endfile)
    {
      server[0]=EOS;
      realm[0]=EOS;
      name[0]=EOS;
      passwd[0]=EOS;

      /* 1st line */
      len = 0;
      while (!endfile && TtaReadByte (file, &c) && c != EOL)
        {
          if (c == 13)
            line[len] = EOS;
          else
            line[len++] = (char)c;
        }
      line[len] = EOS;
      endfile = !(c == EOL);

      if (strcmp (line, "."))
        {
          // goto next password record
          ok = FALSE;
        }
      else
        ok = TRUE;

      /* server/realm line */	
      if (!endfile)
        {
          if (TtaReadByte (file, &c) && c == '"')
            {
              /* server name */
              len = 0;
              while (TtaReadByte (file, &c) && c != EOL && c != '"')
                line[len++] = (char)c;

              if (c == '"')
                {
                  line[len] = EOS;
                  strcpy (server, line);
                  /* skip spaces and the next first " */
                  do
                    {
                      TtaReadByte (file, &c);
                    }
                  while (c != '"' && c != EOL);
                  /* get the realm name */
                  if (c == '"')
                    {
                      len = 0;
                      while (TtaReadByte (file, &c) && c != EOL && c != '"')
                        line[len++] = (char)c;
                      if (c == '"')
                        {
                          line[len] = EOS;
                          strcpy (realm, line);
                          /* go to end of line " */
                          do
                            {
                              TtaReadByte (file, &c);
                            }
                          while (c != EOL);
                        }
                    }
                  endfile = !(c == EOL);
                }
            }
          else
            {
              /* no '"' character in first position */
              while (TtaReadByte (file, &c) && c != EOL)
                line[len++] = (char)c;
              ok = FALSE;
            }
        }
      
      /* name line */
      len = 0;
      while (!endfile && TtaReadByte (file, &c) && c != EOL)
        {
          if (c == 13)
            line[len] = EOS;
          else
            line[len++] = (char)c;
        }
      line[len] = EOS;
      endfile = !(c == EOL);
      strcpy (name, line);

      /* password line */
      len = 0;
      while (!endfile && TtaReadByte (file, &c) && c != EOL)
        {
          if (c == 13)
            line[len] = EOS;
          else
            line[len++] = (char)c;
        }
      line[len] = EOS;
      endfile = !(c == EOL);

      /* Decode the passwd */
      strcpy (inbuf, line);
      EncodeBuf (&inbuf[0], &outbuf[0], FALSE);
      strcpy (passwd, outbuf);

      /* Store a record in password table */
      if (ok)
        NewPasswordTable (realm, server, name, passwd, 0, FALSE);
    }
  TtaReadClose (file);
}

/*----------------------------------------------------------------------
  GetFirtsPasswordsSite: 
  return the first password saved
  ----------------------------------------------------------------------*/
Prop_Passwords_Site * GetFirtsPasswordsSite()
{
  return Pass_FirstSite;
}

/*----------------------------------------------------------------------
  FreePasswordSiteList
  ----------------------------------------------------------------------*/
static void FreePasswordsSiteList()
{
  Prop_Passwords_Site *element, *next = NULL;

  if (Pass_FirstSite == NULL)
    return;

  element = Pass_FirstSite;
  while (element)
    {
      next = element->NextSite;
      TtaFreeMemory (element);
      element = next;
    }

  Pass_FirstSite = NULL;
  return;
}

/*----------------------------------------------------------------------
  UpdatePasswordSiteList: Remove the corresponding entry 
  in the password table
  ----------------------------------------------------------------------*/
void UpdatePasswordsSiteList (int i_site, const char *label_site)
{
  int             i = 0;
  char            table_site [MAX_LENGTH];

  if (i_site > PM_Index)
    return;

  /* Search the corresponding entry in the password table */
  for (i = 1; i <= PM_Index; i++)
    { 
      if (PM_Server[i] != NULL &&  PM_Realm[i] != NULL)
        {
          /* we wearch first the same index */
          if (i == i_site)
            {
              sprintf (table_site, "%s - (%s)", PM_Server[i], PM_Realm[i]);
              /* verification of the site/realm pair */
              if ((label_site != NULL) && (strcmp ((char *)label_site, table_site) == 0))
                RemovePasswordTable (i, TRUE);
            }
        }
    }
}

/*----------------------------------------------------------------------
  LoadPasswordSiteList: load the list of saved sites
  return : number of sites saved
  ----------------------------------------------------------------------*/
int LoadPasswordsSiteList ()
{
  Prop_Passwords_Site *element, *current = NULL;
  int             i;
  int             nb = 0;
  char            site [MAX_LENGTH];

  /* Is the password table already loaded */
  if (PM_Index == 0)
    LoadPasswordTable ();

  /* Clean up the curent list */
  FreePasswordsSiteList ();

  /* Load each server/realm pair */
  for (i = 1; i <= PM_Index; i++)
    { 
      if (PM_Server[i] != NULL &&  PM_Realm[i] != NULL)
        {
          sprintf (site, "%s - (%s)", PM_Server[i], PM_Realm[i]);
          element  = (Prop_Passwords_Site*)TtaGetMemory (sizeof(Prop_Passwords_Site));
          memset (element, 0, sizeof(Prop_Passwords_Site));
          strncpy (element->Site, site, MAX_LENGTH - 1);
          if (Pass_FirstSite == NULL)
            Pass_FirstSite = element; 
          else
            current->NextSite = element;
          current = element;
          nb++;
        }
    }
  return nb;
}

/*----------------------------------------------------------------------
  PasswordManagerStore
  Store the name/password infos
  ----------------------------------------------------------------------*/
void WritePasswordTable ()
{
  int       i;
  char      pwdname[MAX_LENGTH];
  char      pwdname_tmp[MAX_LENGTH];
  char      inbuf[MAX_LENGTH], outbuf[MAX_LENGTH];
  char      server[MAX_LENGTH], realm[MAX_LENGTH];
  char      name[MAX_LENGTH], passwd[MAX_LENGTH];
  FILE     *f;

  if (PM_Index == 0)
    return;

  if (PM_Save)
    {
      /* Open the new password file */
      f = NULL;
      sprintf (pwdname_tmp, "%s%c%s_tmp", TempFileDirectory, DIR_SEP, S_FILE);
      f = TtaWriteOpen (pwdname_tmp);
      if (f == NULL)
        return;
      
      /* Write the password table */
      for (i = 1; i <= PM_Index; i++)
        { 
          server[0]=EOS;
          realm[0]=EOS;
          name[0]=EOS;
          passwd[0]=EOS;
          if (PM_Server[i] != NULL)
            strcpy (server, PM_Server[i]);
          if (PM_Realm[i] != NULL)
            strcpy (realm, PM_Realm[i]);
          if (PM_Name[i] != NULL)
            strcpy (name, PM_Name[i]);
          if (PM_Passwd[i] != NULL)
            strcpy (passwd, PM_Passwd[i]);
	  
          /* Free table */
          RemovePasswordTable (i, FALSE);
	  
          /* Write a password */
          if (server[0] != EOS)
            {
              fprintf (f, "%c%c", '.', EOL);
              fprintf (f, "\"%s\" \"%s\" %c", server, realm, EOL);
              fprintf (f, "%s%c", name, EOL);
              /* Encode the passwd */
              strcpy (inbuf, passwd);
              EncodeBuf (&inbuf[0], &outbuf[0], TRUE);
              fprintf (f, "%s%c", outbuf, EOL);
            }
        }  
      /* Close the new password file */
      if (f != NULL)
        TtaWriteClose (f);
      
      /* Replace the password file with the new one */
      /* Remove the old file */
      sprintf (pwdname, "%s%c%s", TempFileDirectory, DIR_SEP, S_FILE);
      if (TtaFileExist (pwdname))
        TtaFileUnlink (pwdname);
      /* Rename the new file */
      TtaFileRename(pwdname_tmp, pwdname);
    }
  else
    {
      /* Free table */
      for (i = 1; i <= PM_Index; i++)
        RemovePasswordTable (i, FALSE);
    }

  PM_Index = 0;

  // Free the passwords list used by the Preferences  
  FreePasswordsSiteList();

  return;
}

/*----------------------------------------------------------------------
  NewPasswordTable
  Add an new password in  the table of passwords
  ----------------------------------------------------------------------*/
void NewPasswordTable (char *realm, char *server, char *name, char *pwd,
                       int i_auth, ThotBool user)
{
  char      pm_name[MAX_LENGTH];
  char      pm_passwd[MAX_LENGTH];
  ThotBool  new_auth = TRUE;

  if (i_auth != 0)
    {
      /* same name/pwd infos - don't change the record */
      new_auth = FALSE;
      /* get the existing server/realm infos */
      pm_name[0] = EOS;
      pm_passwd[0] = EOS;
      GetPasswordTable (i_auth, &pm_name[0], &pm_passwd[0]);

      if (name[0] != EOS && strcmp (name, pm_name))
        new_auth = TRUE;
      else if (pwd[0] != EOS && strcmp (pwd, pm_passwd))
        {
          TtaFreeMemory (PM_Passwd[i_auth]);
          PM_Passwd[i_auth] = TtaStrdup (pwd);
          if (user)
            PM_Save = TRUE;
        }
    }

  /* Store the new record */
  if (new_auth)
    {
      PM_Index += 1;
      if (PM_Index < MAX_PM_TABLE)
        {
          PM_Server[PM_Index] = TtaStrdup (server);
          PM_Realm[PM_Index] = TtaStrdup (realm);
          PM_Name[PM_Index] = TtaStrdup (name);
          PM_Passwd[PM_Index] = TtaStrdup (pwd);
          if (user)
            PM_Save = TRUE;
        }
      else
        printf ("\nToo Many password in the Password Manager\n");
    }
}

/*----------------------------------------------------------------------
  CleanPasswordTable
  Clean the table of passwords
  ----------------------------------------------------------------------*/
void CleanPasswordTable ()
{
  int        i;

  /* Is the password table loaded ? */
  if (PM_Index == 0)
    LoadPasswordTable ();

  /* Remove every password */
  for (i = 1; i <= PM_Index; i++)
    RemovePasswordTable (i, TRUE);
  return;
}

/*----------------------------------------------------------------------
  RemovePasswordTable
  Remove a password from the table of passwords
  ----------------------------------------------------------------------*/
void RemovePasswordTable (int i_auth, ThotBool user)
{

  if (PM_Index < MAX_PM_TABLE)
    {
      if (PM_Server[i_auth] != NULL)
        {
          TtaFreeMemory (PM_Server[i_auth]);
          PM_Server[i_auth] = NULL;
        }
      if (PM_Realm[i_auth] != NULL)
        {
          TtaFreeMemory (PM_Realm[i_auth]);
          PM_Realm[i_auth] = NULL;
        }
      if (PM_Name[i_auth] != NULL)
        {
          TtaFreeMemory (PM_Name[i_auth]);
          PM_Name[i_auth] = NULL;
        }
      if (PM_Passwd[i_auth] != NULL)
        {
          TtaFreeMemory (PM_Passwd[i_auth]);
          PM_Passwd[i_auth] = NULL;
        }
      if (user)
        PM_Save = TRUE;
    }
  else
    printf ("\nPassword not found in the Password Manager\n");
}

/*----------------------------------------------------------------------
  GetPasswordTable
  Returns the name/password infos for the corresponding entry
  ----------------------------------------------------------------------*/
void GetPasswordTable (int i_auth, char* name, char *passwd)
{
  if (PM_Index == 0)
    return;	

  if (i_auth == 0)
    return;	

  /* Return the right name/password pair */
  strcpy (name, PM_Name[i_auth]);
  strcpy (passwd, PM_Passwd[i_auth]);

  return;
}
/*----------------------------------------------------------------------
  SearchPasswordTable
  Search the corresponding entry for a realm/server pair
  ----------------------------------------------------------------------*/
int SearchPasswordTable (const char *realm, char *server)
{
  int        i;

  if (server == NULL || *server == EOS)
    return 0;

  /* Is the password table already loaded ? */
  if (PM_Index == 0)
    LoadPasswordTable ();

  /* Search the right server/realm pair within the password table */
  for (i = 1; i <= PM_Index; i++)
    { 
      if (PM_Server[i] && !strcmp (server, PM_Server[i]) &&
          (realm == NULL ||
           (PM_Realm[i] && !strcmp (realm, PM_Realm[i]))))
        return i;
    }
  return 0;
}

/*----------------------  Base64 Algorithm ------------------------------*/

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void encode(char *inbuf, char *outbuf)
{
  unsigned char in[3], out[4];
  int i = 0;
  int j = 0;
  int k = 0;
  int len, blocksout = 0;

  int lg;
  lg = strlen (inbuf);
 
  while( j < lg ) {
    len = 0;
    for( i = 0; i < 3; i++ ) {
      if( j < lg ) {
        in[i] = inbuf[j++];
        len++;
      }
      else {
        in[i] = 0;
      }
    }
    if( len ) {
      encodeblock( in, out, len );
      for( i = 0; i < 4; i++ ) {
        outbuf[k++] = out[i];
      }
	    outbuf[k] = EOS;
      blocksout++;
    }
  }
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void decodeblock( unsigned char in[4], unsigned char out[3] )
{   
  out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
  out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
  out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
void decode( char *inbuf, char *outbuf )
{
  unsigned char in[4], out[3], v;
  int len=0;
  int i=0;
  int j=0;
  int k=0;
  int lg;
  lg = strlen (inbuf);

  while( j < lg ) {
    for( len = 0, i = 0; i < 4 && (j < lg); i++ ) {
      v = 0;
      while( (j < lg) && v == 0 ) {
        v = (unsigned char) inbuf [j++];
        v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
        if( v ) {
          v = (unsigned char) ((v == '$') ? 0 : v - 61);
        }
      }
      if( (j <= lg)  && v != 0 ) {
        len++;
        if( v ) {
          in[i] = (unsigned char) (v - 1);
        }
      }
      else {
        in[i] = 0;
      }
    }
    if( len ) {
      decodeblock( in, out );
      for( i = 0; i < len - 1; i++ ) {
	      outbuf[k++] = out[i];
      }
    }
    outbuf[k] = EOS;
  }
}

/*
** Base64 encoding
**
*/
int EncodeBuf (char *inbuf, char *outbuf, ThotBool opt)
{
  int retcode = 0;

  if( opt == TRUE ) {
    encode( inbuf, outbuf );
  }
  else {
    decode( inbuf, outbuf );
  }
    
  return( retcode );
}

