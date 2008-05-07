/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTutil.c : different miscellaneous functions used by the other
 *               annotation modules
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

/* system includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
/* annotlib includes */
#include "annotlib.h"
#include "ANNOTtools_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTschemas_f.h"

/* bookmark includes */
#ifdef BOOKMARKS
#include "bookmarks.h"
#include "f/BMtools_f.h"
#endif /* BOOKMARKS */

/* Amaya includes */
#include "XPointer.h"
#include "XPointerparse_f.h"
#include "init_f.h"
#include "XLinkedit_f.h"
#include "fetchXMLname_f.h"
#include "AHTURLTools_f.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */
  /* libwww includes */
#include "HTHash.h"
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


   
/* schema includes */
#include "Annot.h"
#include "XLink.h"


/* ------------------------------------------------------------
   Global tables implementations
   ------------------------------------------------------------*/
AnnotMetaDataList AnnotMetaData[DocumentTableLength];
#ifdef ANNOT_ON_ANNOT
AnnotThreadList   AnnotThread[DocumentTableLength];
#endif /* ANNOT_ON_ANNOT */



/****************************************************************
 ** 
 ** List Handling functions
 **
 ****************************************************************/
/* ------------------------------------------------------------
   List_addEnd
   Adds a new element to the end of a linked
   list.
   ------------------------------------------------------------*/
void List_addEnd (List **me, void *object)
{
  List *new_;
  List *tmp;

  new_ = (List *) malloc (sizeof (List));
  new_->object = object;
  new_->next = NULL;
  if (!*me)
    *me = new_;
  else
    {
      tmp = *me;
      while (tmp->next)
        tmp = tmp->next;
      tmp->next = new_;
    }
}

/* ------------------------------------------------------------
   List_add
   Adds a new element to the beginning of a linked
   list.
   ------------------------------------------------------------*/
void List_add (List **me, void *object)
{
  List *new_;

  new_ = (List *) malloc (sizeof (List));
  new_->object = object;
  if (!*me)
    new_->next = NULL;
  else
    new_->next = *me;
  *me = new_;
}

/* ------------------------------------------------------------
   List_delFirst
   Deletes the first element of a linked list.
   ------------------------------------------------------------*/
void List_delFirst (List **me)
{
  List *ptr;

  if (*me)
    {
      ptr = (List *) (*me)->next;
      free (*me);
      *me = ptr;
    }
}

/* ------------------------------------------------------------
   List_delCharObj
   Deletes all the elements in a list and the list structure
   ------------------------------------------------------------*/
ThotBool List_delCharObj (void *obj)
{
  if (!obj)
    return FALSE;

  TtaFreeMemory ((char *) obj);
  return TRUE;
}

/* ------------------------------------------------------------
   List_delAll
   Deletes all the elements in a list and the list structure
   ------------------------------------------------------------*/
void List_delAll (List **me, ThotBool (*del_function)(void *))
{
  List *ptr;

  while (*me)
    {
      ptr = (List *) (*me)->next;
      if (del_function && (*me)->object)
        (*del_function) ((*me)->object);
      free (*me);
      *me = ptr;
    }
}

/* ------------------------------------------------------------
   List_delObject
   Removes an element of a linked list.
   ------------------------------------------------------------*/
void List_delObject (List **list, void *object)
{
  List *item = *list;

  while (item && (item->object != object)) {
    list = &item->next;
    item = *list;
  }

  if (item)
    {
      *list = item->next;
      free (item);
    }
}


/* ------------------------------------------------------------
   List_merge
   Concatenates list2 to the end of list1 and returns a 
   pointer to the new list.
   ------------------------------------------------------------*/
List *List_merge (List *list1, List *list2)
{
  List *last_item;

  if (!list1)
    return list2;

  if (!list2)
    return list1;

  last_item = List_getLast (list1);
  last_item->next = list2;

  return (list1);
}

/* ------------------------------------------------------------
   List_getLast
   Returns the last item of a list (if it exists) or
   NULL otherwise.
   ------------------------------------------------------------*/
List *List_getLast (List *list)
{
  List *item = list;

  while (item)
    {
      if (!item->next)
        break;
      item = item->next;
    }

  return (item);
}

/* ------------------------------------------------------------
   List_count
   Returns the number of items in a list
   ------------------------------------------------------------*/
int List_count (List *list)
{
  List *item = list;
  int count = 0;

  while (item)
    {
      count++;
      item = item->next;
    }

  return (count);
}

/* ------------------------------------------------------------
   List_reverse
   Returns a new_list that has the input_list in reverse order.
   Note that the pointers to the lists object element are the
   same. The caller has to be careful when deleting the list.
   ------------------------------------------------------------*/
List * List_reverse (List *input_list)
{
  List *reverse_list = NULL;
  List *cur = input_list;

  while (cur)
    {
      List_add (&reverse_list, cur->object);
      cur = cur->next;
    }
  
  return (reverse_list);
}

/* ------------------------------------------------------------
   AnnotList_localCount
   Returns the number of local annotations in an annotation  list
   ------------------------------------------------------------*/
int AnnotList_localCount (List *annot_list)
{
  List *item = annot_list;
  int count = 0;
  AnnotMeta *annot;
  
  while (item)
    {
      annot = (AnnotMeta *) item->object;
      /* local annotations have a null URI if they were newly
         created this session, otherwise they have a file: URI */
      if (!annot->annot_url || IsFilePath(annot->annot_url))
        count++;
      item = item->next;
    }
  return (count);
}

/*---------------------------------------------------------------
  AnnotFilter_toggleAll
  Changes the show settings for all the filters of 
  a given type.
  ------------------------------------------------------------------*/
void AnnotFilter_toggleAll (Document doc, SelType selector, ThotBool show)
{
  List *list_item;
  AnnotFilterData *filter;

  /* change the filter metadata first */
  switch (selector)
    {
    case BY_AUTHOR:
      list_item = AnnotMetaData[doc].authors;
      break;
    case BY_TYPE:
      list_item = AnnotMetaData[doc].types;
      break;
    case BY_SERVER:
      list_item = AnnotMetaData[doc].servers;
      break;
    default:
      list_item = NULL;
      break;
    }
  
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter)
        filter->show = show;
      list_item = list_item->next;
    }
}

/*-----------------------------------------------------------------------
  AnnotFilter_update
  -----------------------------------------------------------------------*/
static void AnnotFilter_update (Document source_doc, AnnotMeta *annot)
{
  char       *tmp;
  char       server[MAX_LENGTH];

  if (!(annot->is_visible) || annot->is_orphan)
    return;

  AnnotFilter_add (&AnnotMetaData[source_doc], BY_TYPE, annot->type, annot);
  if (annot->annot_url)
    tmp = annot->annot_url;
  else
    tmp = annot->body_url;

  if (tmp)
    { /* @@ when creating a new annot, we don't yet know the URL;
         perhaps we should use the POST server name here? */
      GetServerName (tmp, server);
      AnnotFilter_add (&AnnotMetaData[source_doc], BY_SERVER, server, annot);
    }
  else
    server[0] = EOS;

  if (annot->author)
    {
      tmp = (char *)TtaGetMemory (strlen (annot->author) + strlen (server) + 4);
      sprintf (tmp, "%s@%s", annot->author, server);
      AnnotFilter_add (&AnnotMetaData[source_doc], BY_AUTHOR, tmp, annot);
      TtaFreeMemory (tmp);
    }
}

/* ------------------------------------------------------------
   AnnotFilter_add
   Adds a new element to the beginning of a linked
   list if it doesn't exist in the list.
   ------------------------------------------------------------*/
void AnnotFilter_add (AnnotMetaDataList *annotMeta, SelType type, void *object, AnnotMeta *annot)
{
  List **me;
  List *new_;
  ThotBool isString = TRUE;
  AnnotFilterData *filter;

  if (!object || !annot)
    return;

  switch (type)
    {
    case BY_AUTHOR:
      me = &annotMeta->authors;
      isString = TRUE;
      break;

    case BY_TYPE:
      me = &annotMeta->types;
      isString = FALSE;
      break;

    case BY_SERVER:
      me = &annotMeta->servers;
      isString = TRUE;
      break;

    default:
      me = NULL;
      break;
    }

  /* no selection or object already in the filter */
  if (!me 
      || (*me && AnnotFilter_search (*me, object, isString)))
    return;

  /* initialize the filter */
  filter = (AnnotFilterData*)TtaGetMemory (sizeof (AnnotFilterData));
  filter->object = (char *)(isString ? TtaStrdup ((char*)object) : object);
  filter->show = TRUE;

  /* and now add it to the list */
  new_ = (List *) malloc (sizeof (List));
  new_->object = (void *) filter;
  if (!*me)
    new_->next = NULL;
  else
    new_->next = *me;
  *me = new_;
}

/*------------------------------------------------------------
  AnnotFilter_delete
  Deletes an annotation from a filter list. The object 
  field is deleted with the del_function passed as a
  parameter (if it exists).
  Returns TRUE if something was deleted, FALSE otherwise
  ------------------------------------------------------------*/
ThotBool AnnotFilter_delete (List **list, AnnotMeta *annot, ThotBool (*del_function)(void *))
{
#if 0
  ThotBool result;

  List *list_item, *prev;
  AnnotFilterData *filter;

  list_item = *list;
  prev = NULL;
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter->annot == annot)
        break;
      prev = list_item;
      list_item = list_item->next;
    }

  if (list_item)
    {
      result = TRUE;

      /* fix the pointers */
      if (prev == NULL)
        *list = list_item->next;
      else
        prev->next = list_item->next;
      
      /* free allocated memory */
      if (del_function && filter->object)
        (*del_function) (filter->object);
      TtaFreeMemory (filter);
      TtaFreeMemory (list_item);
    }
  else
    result = FALSE;

  return (result);
#endif
  return FALSE;
}

/*------------------------------------------------------------
  AnnotFilter_search
  Returns list item that contains the object
  ------------------------------------------------------------*/
List *AnnotFilter_search (List *list, void *object, ThotBool isString)
{
  List *list_item = list;
  AnnotFilterData *filter;
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (isString)
        {
          if (!strcasecmp (filter->object, (char*)object))
            break;
        }
      else
        {
          if (filter->object == object)
            break;
        }

      list_item = list_item->next;
    }

  return (list_item);
}

/*------------------------------------------------------------
  AnnotFilter_status
  Verifies the visibility of  all the annotations related to
  docid doc, and that are related to the selector object.

  Returns 0 if all annotations are hidden.
  Returns 1 if all the annotatios are visible.
  Returns 2 if some annotations are visible
  ------------------------------------------------------------*/
int AnnotFilter_status (Document doc, SelType selector, void *object)
{
  List *list_item;
  int show;
  ThotBool compare;
  AnnotMeta *annot;
  char server[MAX_LENGTH];
  char *tmp;

  if (!object)
    return 0;

  show = -1;
  list_item = AnnotMetaData[doc].annotations;
  for (; list_item; list_item = list_item->next)
    {
      annot = (AnnotMeta *)list_item->object;
      /* skip those annotations that are not shown in the formatted
         view (or that are invisible to the user */
      if (!(annot->is_visible) || annot->is_orphan)
        continue;
      
      switch (selector)
        {
        case BY_TYPE:
          compare = (annot->type == object);
          break;
        case BY_AUTHOR:
          compare = (strncmp (annot->author, (char *) object,
                              strlen (annot->author)) == 0);
          break;
        case BY_SERVER:
          if (annot->annot_url)
            tmp = annot->annot_url;
          else
            tmp = annot->body_url;
          GetServerName (tmp, server);
          compare = !strcmp (server, (char *) object);
          break;

        default:
          compare = 0;
          break;
        }
      if (compare)  /* update the status */
        {
          if (show == -1)
            /* it's the first one we compare */
            show = (annot->show) ? 1 : 0;
          else if ((annot->show && show == 0)
                   || (!(annot->show) && show == 1))
            /* different status */
            {
              show = 2;
              break;
            }
        }
    }
  return ((show == -1) ? 1 : show);
}

/*------------------------------------------------------------
  AnnotFilter_show
  Returns a boolean saying if a filter element containing
  a given object should be shown. If no filter element is
  found, it returns TRUE.
  ------------------------------------------------------------*/
ThotBool AnnotFilter_show (List *list, void *object)
{
  List *list_item = list;
  AnnotFilterData *filter;

  if (!object)
    return TRUE;

  list_item = AnnotFilter_search (list, object, FALSE);
  if (!list_item)
    return TRUE;
  filter = (AnnotFilterData *) list_item->object;
  if (filter)
    return filter->show;
  else
    return TRUE;
}

/*------------------------------------------------------------
  AnnotFilter_showServer
  Returns a boolean saying if a filter element containing
  a given object should be shown. If no filter element is
  found, it returns TRUE.
  ------------------------------------------------------------*/
ThotBool AnnotFilter_showServer (List *list, char *url)
{
  List *list_item = list;
  AnnotFilterData *filter;
  char server[MAX_LENGTH];

  /* we first normalize the url name to get the server */
  GetServerName (url, server);

  list_item = AnnotFilter_search (list, server, TRUE);
  if (!list_item)
    return TRUE;
  filter = (AnnotFilterData *) list_item->object;
  if (filter)
    return filter->show;
  else
    return TRUE;
}

/*------------------------------------------------------------
  AnnotFilter_showAuthor
  Returns a boolean saying if a filter element containing
  a given object should be shown. If no filter element is
  found, it returns TRUE.
  ------------------------------------------------------------*/
ThotBool AnnotFilter_showAuthor (List *list, char *author, char *url)
{
  List *list_item = list;
  AnnotFilterData *filter;
  char server[MAX_LENGTH];
  char *tmp;
  ThotBool result;

  if (!author)
    return TRUE;

  /* we first normalize the url name to get the server */
  GetServerName (url, server);

  tmp = (char *)TtaGetMemory (strlen (author) + strlen (server) + 4);
  sprintf (tmp, "%s@%s", author, server);

  list_item = AnnotFilter_search (list, tmp, TRUE);
  if (!list_item)
    result = TRUE;
  else 
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter)
        result = filter->show;
      else
        result = TRUE;
    }
  TtaFreeMemory (tmp);

  return result;
}

/*------------------------------------------------------------
  AnnotFilter_deleteAll
  Deletes allannotation filter lists for a given source
  document.
  ------------------------------------------------------------*/
void AnnotFilter_deleteAll (Document doc)
{
  if (AnnotMetaData[doc].authors)
    AnnotFilter_free (AnnotMetaData[doc].authors, List_delCharObj);
  AnnotMetaData[doc].authors = NULL;
  if (AnnotMetaData[doc].types)
    AnnotFilter_free (AnnotMetaData[doc].types, NULL);
  AnnotMetaData[doc].types = NULL;
  if (AnnotMetaData[doc].servers)
    AnnotFilter_free (AnnotMetaData[doc].servers, List_delCharObj);
  AnnotMetaData[doc].servers = NULL;
}

/*------------------------------------------------------------
  AnnotFilter_build
  Builds the annotation filter list for a given source
  document.
  ------------------------------------------------------------*/
void AnnotFilter_build (Document doc)
{
  List *item;
  AnnotMeta *annot;

  /* delete any previous filters */
  AnnotFilter_deleteAll (doc);

  item = AnnotMetaData[doc].annotations;
  while (item)
    {
      annot = (AnnotMeta *) item->object;
      AnnotFilter_update (doc, annot);
      item = item->next;
    }
}

/*------------------------------------------------------------
  AnnotList_search
  Returns the list item that contains the object
  ------------------------------------------------------------*/
List *AnnotList_search (List *list, const char *object)
{
  List *item = list;

  while (item && (strcasecmp ((char *) item->object, (char *) object)))
    {
      item = item->next;
    }

  return (item);
}

/*------------------------------------------------------------
  AnnotList_searchAnnot
  Returns the annot item that points to the same url
  ------------------------------------------------------------*/
AnnotMeta *AnnotList_searchAnnot (List *list, char *url, AnnotMetaDataSearch searchType)
{
  List *item = list;
  AnnotMeta *annot = NULL;
  ThotBool found = FALSE;
  char *ptr = NULL;

  /* due to an earlier code bug, some bad data exists.  Protect from it */
  if (!url)
    return NULL;

  while (item)
    {
      annot = (AnnotMeta *) item->object;

      switch (searchType)
        {
        case AM_ANNOT_URL:
          ptr = annot->annot_url;
          break;

        case AM_BODY_URL:
          ptr = annot->body_url;
          break;

#if 0 /* This case is no longer necessary given FixFileURL() */
        case AM_BODY_FILE:
          if (annot->body_url && IsFilePath (annot->body_url))
            ptr = annot->body_url + sizeof ("file://") - 1;
          else
            ptr = annot->body_url;
          break;
#endif

        case AM_ANAME:
          ptr = annot->name;
          break;
        }

      if (ptr && !strcasecmp (ptr, url))
        {
          found = TRUE;
          break;
        }
      item = item->next;
    }

  return (found) ? annot : NULL;
}

/*------------------------------------------------------------
  AnnotList_searchAnnotBodyURL
  Returns the URL of annot body that corresponds to annot_url
  or NULL if it doesn't exist
  ------------------------------------------------------------*/
char *AnnotList_searchAnnotBodyURL (Document source_doc, char *annot_url)
{
  AnnotMeta *annot;

  annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                 annot_url,
                                 AM_ANNOT_URL);
#ifdef ANNOT_ON_ANNOT
  if (!annot && AnnotThread[source_doc].annotations)
    annot = AnnotList_searchAnnot (AnnotThread[source_doc].annotations,
                                   annot_url,
                                   AM_ANNOT_URL);
#endif /* ANNOT_ON_ANNOT */

  if (annot && annot->body_url)
    return annot->body_url;
  else
    return NULL;
}

/*------------------------------------------------------------
  AnnotList_searchAnnotURL
  Returns the URL of annot that corresponds to annot_url
  or NULL if it doesn't exist
  ------------------------------------------------------------*/
char *AnnotList_searchAnnotURL (Document source_doc, char *body_url)
{
  AnnotMeta *annot;
  char *ptr;

  if (!IsW3Path (body_url) && !IsFilePath (body_url))
    ptr = ANNOT_MakeFileURL (body_url);
  else
    ptr = body_url;

  annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                 ptr,
                                 AM_BODY_URL);
#ifdef ANNOT_ON_ANNOT
  if (!annot && AnnotThread[source_doc].annotations)
    annot = AnnotList_searchAnnot (AnnotThread[source_doc].annotations,
                                   ptr,
                                   AM_BODY_URL);
#endif /* ANNOT_ON_ANNOT */

  if (ptr != body_url)
    TtaFreeMemory (ptr);

  if (annot && annot->annot_url)
    return annot->annot_url;
  else
    return NULL;
}

/*------------------------------------------------------------
  AnnotList_searchBody
  Returns the annot meta corresponding to the body URL
  or NULL if it doesn't exist
  ------------------------------------------------------------*/
AnnotMeta *AnnotList_searchBody (Document annot_doc)
{
  AnnotMeta *annot;
  char *body_url;
  char *ptr;
  Document source_doc;
  
  if (DocumentTypes[annot_doc] != docAnnot)
    return NULL;

  source_doc = DocumentMeta[annot_doc]->source_doc;
  body_url = DocumentURLs[annot_doc];

  if (!IsW3Path (body_url) && !IsFilePath (body_url))
    ptr = ANNOT_MakeFileURL (body_url);
  else
    ptr = body_url;

  annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                 ptr,
                                 AM_BODY_URL);
#ifdef ANNOT_ON_ANNOT
  if (!annot && AnnotThread[source_doc].annotations)
    annot = AnnotList_searchAnnot (AnnotThread[source_doc].annotations,
                                   ptr,
                                   AM_BODY_URL);
#endif /* ANNOT_ON_ANNOT */

  if (ptr != body_url)
    TtaFreeMemory (ptr);

  return (annot);
}

/*------------------------------------------------------------
  AnnotList_delAnnot
  Searches for annotation with URL url and, if found, deletes it
  and returns TRUE. Returns FALSE otherwise.
  ------------------------------------------------------------*/
ThotBool AnnotList_delAnnot (List **list, char *url, ThotBool useAnnotUrl)
{
  List *item, *prev;
  AnnotMeta *annot = NULL;
  ThotBool found = FALSE;
  char *ptr;

  prev = NULL;
  item = *list;
  while (item)
    {
      annot = (AnnotMeta *) item->object;
      /* @@ this crashes... why? */
      if (useAnnotUrl && annot->annot_url)
        ptr = annot->annot_url;
      else
        ptr = annot->body_url;

      if (ptr && !strcasecmp (ptr, url))
        {
          found = TRUE;
          break;
        }
      prev = item;
      item = item->next;
    }

  if (found && annot)
    {
      /* update the pointers */
      if (prev)
        prev->next = item->next;
      else
        *list = item->next;
      /* erase the annotation body */
      if (IsFilePath (annot->body_url))
        {
          ptr = (char *)TtaGetMemory (strlen (annot->body_url) + 1);
          NormalizeFile (annot->body_url, ptr, AM_CONV_NONE);
          TtaFileUnlink (ptr);
          TtaFreeMemory (ptr);
        }
      /* delete the annotation */
      Annot_free (annot);
      /* delete the list item */
      TtaFreeMemory (item);
    }

  return (found);
}

/*------------------------------------------------------------
  AnnotThread_UpdateReplyTo
  ------------------------------------------------------------*/
int AnnotThread_UpdateReplyTo (List *thread_list,
                               char *new_url,
                               char *prev_url)
{
#ifdef ANNOT_ON_ANNOT
  AnnotMeta *annot;
  List *item;
  int count;

  item = thread_list;
  annot = (AnnotMeta *) item->object;

  count = 0;
  while (item)
    {
      annot = (AnnotMeta *) item->object;
      if (annot->inReplyTo)
        {
          /* replace the root of Thread */
          if (!strcasecmp (annot->rootOfThread, prev_url))
            {
              TtaFreeMemory (annot->rootOfThread);
              annot->rootOfThread = TtaStrdup (new_url);
              count++;
            }

          /* replace the reply to */
          if (!strcasecmp (annot->inReplyTo, prev_url))
            {
              TtaFreeMemory (annot->inReplyTo);
              annot->inReplyTo = TtaStrdup (new_url);
              count++;
            }
          /* replace the source_url */
          if (!strcasecmp (annot->source_url, prev_url))
            {
              TtaFreeMemory (annot->source_url);
              annot->source_url = TtaStrdup (new_url);
              count++;
            }
        }
      item = item->next;
    }
  return (count);
#else
  return 0;
#endif /* ANNOT_ON_ANNOT */
}

/*------------------------------------------------------------
  AnnotThread_UpdateAnnotates
  ------------------------------------------------------------*/
int AnnotThread_UpdateAnnotates (List *annot_list,
                                 char *new_url,
                                 char *prev_url)
{
#ifdef ANNOT_ON_ANNOT
  AnnotMeta *annot;
  List *item;
  int count;

  item = annot_list;

  count = 0;
  while (item)
    {
      annot = (AnnotMeta *) item->object;
      if (!strcasecmp (annot->source_url, prev_url))
        {
          TtaFreeMemory (annot->source_url);
          annot->source_url = TtaStrdup (new_url);
          count++;
        }
      item = item->next;
    }
  return (count);
#else
  return 0;
#endif /* ANNOT_ON_ANNOT */
}

/*------------------------------------------------------------
  Annot_searchDocument
  Returns the doc reference of the document that
  contains the body_url or NONE if not found.
  ------------------------------------------------------------*/
Document Annot_searchDocument (char *body_url)
{
  int i;
  char *tmp;

  if (IsFilePath (body_url))
    {
      tmp = TtaStrdup (body_url);
      WWWToLocal (tmp);
    }
  else
    tmp = body_url;

  for (i = 1; i < DocumentTableLength; i++)
    {
      if (DocumentURLs[i] && !strcmp (DocumentURLs[i], tmp))
        break;
    }

  if (tmp != body_url)
    TtaFreeMemory (tmp);

  return (i == DocumentTableLength) ? (Document) None : (Document) i;
}

/*------------------------------------------------------------
  AnnotThread_searchRoot
  Returns the doc reference of the thread that corresponds to 
  the root URL.
  ------------------------------------------------------------*/
Document AnnotThread_searchRoot (char *root)
{
#ifdef ANNOT_ON_ANNOT
  int i;

  for (i = 1; i < DocumentTableLength; i++)
    {
      if (!AnnotThread[i].rootOfThread)
        continue;
      
      if (!strcmp (AnnotThread[i].rootOfThread, root))
        break;
    }

  return (i == DocumentTableLength) ? 0 : i;
#else
  return 0;
#endif /* ANNOT_ON_ANNOT */
}

/*------------------------------------------------------------
  AnnotThread_searchThreadDoc
  Returns the docid of the thread to which an annotation belongs 
  or 0 if it's not found.
  ------------------------------------------------------------*/
Document AnnotThread_searchThreadDoc (char *annot_url)
{
#ifdef ANNOT_ON_ANNOT
  int i;
  AnnotMetaDataSearch searchType;
  char *tmp_url;

  if (IsW3Path (annot_url) || IsFilePath (annot_url))
    tmp_url = annot_url;
  else
    tmp_url = LocalToWWW (annot_url);
  searchType = AM_BODY_URL;

  for (i = 0; i < DocumentTableLength; i++)
    {
      if (!AnnotThread[i].rootOfThread)
        continue;
      if (AnnotList_searchAnnot (AnnotThread[i].annotations, 
                                 tmp_url, searchType))
        break;
    }

  if (tmp_url != annot_url)
    TtaFreeMemory (tmp_url);

  return (i == DocumentTableLength) ? 0 : i;
#else
  return 0;
#endif /* ANNOT_ON_ANNOT */
}

/*------------------------------------------------------------
  Annot_GetMDate
  Returns the modified date stored in the AnnotMeta structure
  ------------------------------------------------------------*/
char * Annot_GetMDate (void *object)
{
  return ((AnnotMeta *) object)->mdate;
}

#ifdef ANNOT_ON_ANNOT
/**********************************
 Thread sort algorithm adapted from
 Jamie Zawinski's <jwz@jwz.org>
 http://www.jwz.org/doc/threading.html
*********************************/


typedef struct _Container
{
  struct _Container *parent; /* id of the parent of this item */
  struct _Container *child;  /* children linked to this thread item*/
  struct _Container *next;   /* sibling of this thread item */
  void   *object;            /* the object stored in this container */
} Container;

HTHashtable *id_table;

static Container * ThreadItem_new (void)
{
  Container *me;

  me = (Container*)TtaGetMemory (sizeof (Container));
  memset (me, 0, sizeof (Container));
  return (me);
}

static void  ThreadItem_delete (Container *item)
{
  Container *me = item;

  if (me->child)
    ThreadItem_delete (me->child);
  if (me->next)
    ThreadItem_delete (me->next);
  TtaFreeMemory (me);
}

/*------------------------------------------------------------
  ConvertContainerToList
  ------------------------------------------------------------*/
static void ConvertContainerToList (List **result, Container *container)
{
  Container *container_tmp;
  Container *tmp2;
  
  container_tmp = container;
  while (container_tmp)
    {
      if (container_tmp->object)
        List_addEnd (result, container_tmp->object);
      if (container_tmp->child)
        {
          ConvertContainerToList (result, container_tmp->child);
          container_tmp->child = NULL;
        }
      tmp2 = container_tmp;
      container_tmp = container_tmp->next;
      tmp2->next = NULL;
      ThreadItem_delete (tmp2);
    }
}

/*------------------------------------------------------------
  Container_promoteChild
  Put a child at the end of the parent list.
  ------------------------------------------------------------*/
static void Container_promoteEntry (Container **parent, Container *tmp_entry)
{
  Container *tmp;

  if (tmp_entry == *parent)
    return;

  if (*parent == NULL)
    *parent = tmp_entry;
  else
    {
      tmp = *parent;
      while (tmp->next)
        tmp = tmp->next;
      tmp->next = tmp_entry;
      tmp_entry->parent = (*parent)->parent;
      tmp_entry->next = NULL;
    }
}

/*------------------------------------------------------------
  Container_prune
  Remove the empty containers (those that don't have
  any annotations) and upgrade their children.
  ------------------------------------------------------------*/
static void Container_prune (Container **parent)
{
  Container *tmp_entry, *previous, *child;
  
  tmp_entry = *parent;
  previous = NULL;
  while (tmp_entry)
    {
      if (!tmp_entry->object && tmp_entry->child)
        {
          if (!tmp_entry->child->next)
            {
              /* empty container has only one child. Promote it to the same level 
                 and destroy the parent */
              child = tmp_entry->child;
              child->parent = tmp_entry->parent;
              if (!previous)
                *parent = child;
              else
                previous->next = child;
              child->next = tmp_entry->next;
              tmp_entry->child = NULL;
              tmp_entry->next = NULL;
              ThreadItem_delete (tmp_entry);
              tmp_entry = child;
              continue;
            }
          else if (tmp_entry->parent)
            {
              /* it's not the only child, promote all the children to the
                 parents level, except if it would become parent */
              child = tmp_entry->child;
              if (tmp_entry == *parent)
                *parent = child;
              else
                previous->next = child;
              /* fix the links of the child to the parent.
                 insert the next tmp_entry element at the end
                 of the last child. */
              while (child)
                {
                  child->parent = tmp_entry->parent;
                  if (!child->next)
                    {
                      child->next = tmp_entry->next;
                      break;
                    }
                  child = child->next;
                }
              child = tmp_entry->child;
              tmp_entry->child = NULL;
              tmp_entry->next = NULL;
              ThreadItem_delete (tmp_entry);
              tmp_entry = child;
              continue;
            }
        }
      else if (tmp_entry->child)
        Container_prune (&tmp_entry->child);
      if (!tmp_entry)
        break;
      previous = tmp_entry;
      tmp_entry = tmp_entry->next;
    }
}

/*------------------------------------------------------------
  Container_sortByDate
  Sorts the annotations siblings in the container list
  by most recent date.
  ------------------------------------------------------------*/
static void Container_sortByDate (Container **list, char * (*get_date_function)(void *))
{
  time_t date_cur;
  time_t date_next;
  ThotBool swap;
  Container *entry_cur;
  Container *entry_next;
  Container *previous;

  if (!get_date_function)
    return;

  /* simple bubble sort */

  while (1)
    {
      int count;
      swap = FALSE;
      previous = NULL;
      entry_cur = *list;
      count = 0;
      while (entry_cur && entry_cur->next)
        {
          entry_next = entry_cur->next;
          date_cur = StrDateToCalTime ((*get_date_function) (entry_cur->object));
          date_next = StrDateToCalTime ((*get_date_function) (entry_next->object));

#if 0
          printf ("%d: date cur %d %cdate next %d\n", count++, date_cur, 
                  (date_cur > date_next) ? '>' : '<',  date_next);
#endif
          /* sort by most recent date */
          if (date_cur < date_next)
            {
#if 0
              printf ("switching date cur and date next\n");
              printf ("before:\n\tprevious = %p, previous-next = %p\n"
                      "entry_cur = %p, entry_cur_next = %p\n"
                      "entry_next = %p, entry_next->next = %p\n",
                      previous, (previous) ? previous->next : NULL,
                      entry_cur, entry_cur->next,
                      entry_next, entry_next->next);
#endif
              /* swap cur and next */
              swap = TRUE;
              entry_cur->next = entry_next->next;
              entry_next->next = entry_cur;

              /* update previous */
              if (previous)
                previous->next = entry_next;
              else
                {
                  *list = entry_next;
                  entry_next->parent->child = entry_next;
                }

#if 0
              printf ("after:\n\tprevious = %p, previous-next = %p\n"
                      "entry_cur = %p, entry_cur_next = %p\n"
                      "entry_next = %p, entry_next->next = %p\n",
                      (previous) ? previous : NULL, (previous) ? previous->next : NULL,
                      entry_cur, entry_cur->next,
                      entry_next, entry_next->next);
#endif
              previous = entry_next;
            }
          else
            {
              previous = entry_cur;
              entry_cur = entry_next;
            }
        }
      if (!swap)
        break;
    }
}

/*------------------------------------------------------------
  AnnotThread_markOrphan
  Marks as orphan all the top annotation entries following the
  root entry.
  ------------------------------------------------------------*/
static void  AnnotThread_markOrphan (Container *root)
{
  Container *tmp_entry;
  Container *tmp_entry2;

  if (!root)
    return;

  /* skip the first entry, as it holds all the thread items that
     have been accounted for */
  tmp_entry = root->next;
  /* for each sibling, find the first annotation entry and mark
     it as orphan */
  while (tmp_entry)
    {
      tmp_entry2 = tmp_entry;
      while (tmp_entry2 && tmp_entry2->object == NULL)
        tmp_entry2 = tmp_entry2->child;
      if (tmp_entry2)
        ((AnnotMeta *) tmp_entry2->object)->is_orphan_item = TRUE;
      tmp_entry = tmp_entry->next;
    }
}
#endif /* ANNOT_ON_ANNOT */

/*------------------------------------------------------------
  AnnotThread_sortThreadList
  Sorts the thread list pointed by annotlist by InReplyTos.
  ------------------------------------------------------------*/
void AnnotThread_sortThreadList (List **thread_list)
{
#ifdef ANNOT_ON_ANNOT
  List *annot_list, *list_cur;
  AnnotMeta *annot_cur;
  int i;
  Container *cur_entry;
  Container *par_entry;
  Container *tmp_entry;
  Container *root;
  HTArray *keys;
  char *url;

  list_cur = *thread_list;

  if (!list_cur)
    return;

  id_table = HTHashtable_new (0);

  /* create a container for the root */
  annot_cur = (AnnotMeta *) list_cur->object;
  root = ThreadItem_new ();
  HTHashtable_addObject (id_table, annot_cur->rootOfThread, root);
  while (list_cur)
    {      
      annot_cur = (AnnotMeta *) list_cur->object;

      /* reset the orphan annotation status */
      annot_cur->is_orphan_item = FALSE;

      /* create the message container */
      /* use either body_url or annot_url */
      if (!annot_cur->annot_url || IsFilePath (annot_cur->annot_url))
        url = annot_cur->body_url;
      else
        url = annot_cur->annot_url;
      /* url = annot_cur->annot_url; */
      cur_entry = (Container *) HTHashtable_object (id_table, url);
      if (!cur_entry)
        {
          cur_entry = ThreadItem_new ();
          HTHashtable_addObject (id_table, url, cur_entry);
        }
      cur_entry->object = (void *) annot_cur;
	
      /* insert the message container to its parent */
      par_entry = (Container *) HTHashtable_object (id_table, annot_cur->inReplyTo);
      if (!par_entry)
        {
          par_entry = ThreadItem_new ();
          HTHashtable_addObject (id_table, annot_cur->inReplyTo, par_entry);
        }	
      /* add the link from the child to the parent */
      cur_entry->parent = par_entry;
      
      /* insert it with its siblings */
      if (!par_entry->child)
        par_entry->child = cur_entry;
      else
        {
          tmp_entry = par_entry->child;
          while (tmp_entry->next)
            tmp_entry = tmp_entry->next;
          tmp_entry->next = cur_entry;
        }
      list_cur = list_cur->next;
    }

  /* find the root set */
  keys = HTHashtable_keys (id_table);
  for (i = 0; i < HTArray_size (keys); i++)
    {
      tmp_entry = (Container *) HTHashtable_object (id_table, (char *)HTArray_data (keys)[i]);
      if (tmp_entry->parent == NULL)
        {
          if (tmp_entry == root)
            continue;
          else if (tmp_entry->object || tmp_entry->child)
            /* promote to parent */
            Container_promoteEntry (&root, tmp_entry);
          else if (tmp_entry->child == NULL)
            /* destroy */
            {
              tmp_entry->next = NULL;
              ThreadItem_delete (tmp_entry);
            }
        }
    }
  for (i = 0; i< HTArray_size (keys); i++)
    HT_FREE (HTArray_data(keys)[i]);

  HTArray_delete (keys);
  HTHashtable_delete(id_table);

  /* prune the empty containers */
  tmp_entry = root;
  while (tmp_entry)
    {
      Container_prune (&tmp_entry->child);
      tmp_entry = tmp_entry->next;
    }

  /* all is sorted by reply to, now sort it according to dates */
  tmp_entry = root;
  while (tmp_entry)
    {
      Container *tmp_child;
      
      tmp_child = tmp_entry->child;
      while (tmp_child)
        {
          Container_sortByDate (&tmp_child, Annot_GetMDate);
          tmp_child = tmp_child->child;
        }
      tmp_entry = tmp_entry->next;
    }

  /* mark the root orphan annotations as such. After all the above sorts,
     these are all the siblings of the first container */
  AnnotThread_markOrphan (root);

  /* copy the result to annot_list. The children of root are live replies.
     The brothers of root have messages that have lost their in-reply-to parents */
  annot_list = NULL;
  tmp_entry = root;
  while (tmp_entry)
    {
      ConvertContainerToList (&annot_list, tmp_entry->child);
      tmp_entry = tmp_entry->next;
    }
  *thread_list = annot_list;
#endif /* ANNOT_ON_ANNOT */
}

#ifdef BOOKMARKS
/*------------------------------------------------------------
  BM_recursiveSort
  recursively sorts the bookmark containers
  ------------------------------------------------------------*/
static void BM_recursiveSort (Container *root)
{
  Container *topics, *topics_cur;
  Container *bookmarks, *bookmarks_cur;
  Container *tmp_entry, *next;
  
  tmp_entry = root->child;
  topics_cur = bookmarks_cur = NULL;
  if (!tmp_entry)
    return;

  /* 
  ** first sort all the children at this level
  */
  

  /* separate the topics and the bookmarks, for putting topics before bookmarks */
  topics = NULL;
  bookmarks = NULL;
  while (tmp_entry)
    {
      next = tmp_entry->next;
      if (BM_IsTopic (tmp_entry->object))
        {
          if (!topics)
            topics = tmp_entry;
          else
            topics_cur->next = tmp_entry;
          topics_cur = tmp_entry;
          topics_cur->next = NULL;
        }
      else 
        {
          if (!bookmarks)
            bookmarks = tmp_entry;
          else
            bookmarks_cur->next = tmp_entry;
          bookmarks_cur = tmp_entry;
          bookmarks_cur->next = NULL;
        }
      tmp_entry = next;
    }

  /* sort topics and bookmarks separately */
  tmp_entry = topics;
  Container_sortByDate (&tmp_entry, BM_GetMDate);
  /* the first element may have changed, so we update it */
  topics = tmp_entry;
  tmp_entry = bookmarks;
  Container_sortByDate (&tmp_entry, BM_GetMDate);
  bookmarks = tmp_entry;

  /* link the bookmarks back to the topics */
  if (topics)
    {
      root->child = topics;
      if (bookmarks) {
        tmp_entry = topics;
        while (tmp_entry->next)
          tmp_entry = tmp_entry->next;
        tmp_entry->next = bookmarks;
      }
    }
  else
    root->child = bookmarks;

  /* now sort each of its children recursively */
  tmp_entry = root->child;
  while (tmp_entry)
    {
      if (tmp_entry->child)
        BM_recursiveSort (tmp_entry);
      tmp_entry = tmp_entry->next;
    }
}
#endif /* BOOKMARKS */

/*------------------------------------------------------------
  BM_bookmarksSort
  Sorts the bookmark list according to parents
  ------------------------------------------------------------*/
void BM_bookmarksSort (List **bookmark_list)
{
#ifdef BOOKMARKS
  List *list_cur, *list_tmp;
  BookmarkP bookmark_cur;
  int i;
  Container *cur_entry;
  Container *par_entry;
  Container *tmp_entry;
  Container *root;
  HTArray *keys;
  char *url;

  list_cur = *bookmark_list;

  if (!list_cur)
    return;

  id_table = HTHashtable_new (0);

  /* create a container for the root */
  bookmark_cur = (BookmarkP) list_cur->object;
  root = ThreadItem_new ();
  /* @@ JK this doesn't seem useful now */
  HTHashtable_addObject (id_table, "", root);
  while (list_cur)
    {      
      bookmark_cur = (BookmarkP) list_cur->object;

      /* create the message container */
      if (bookmark_cur->bm_type == BME_TOPIC)
        url = bookmark_cur->self_url;
      else
        {
          url = (char*) TtaGetMemory (strlen (bookmark_cur->self_url) 
                                      + strlen (bookmark_cur->parent_url)
                                      + 2);
          sprintf (url, "%s:%s", bookmark_cur->self_url, bookmark_cur->parent_url);
        }
      cur_entry = (Container *) HTHashtable_object (id_table, url);
      if (bookmark_cur->bm_type != BME_TOPIC && cur_entry)
        {
          printf ("Warning, cur_entry already exists for url %s\n", url);
          TtaFreeMemory (url);
          list_cur = list_cur->next;
          continue;
        }
      if (!cur_entry)
        {
          cur_entry = ThreadItem_new ();
          HTHashtable_addObject (id_table, url, cur_entry);
        }

      cur_entry->object = (void *) bookmark_cur;
      if (bookmark_cur->bm_type != BME_TOPIC)
        TtaFreeMemory (url);

      /* insert the message container to its parent */
      /* @@ JK: and if there's no parent? We put it in a blank space? */
      if (bookmark_cur->parent_url)
        url = bookmark_cur->parent_url;
      else
        url = "";

      par_entry = (Container *) HTHashtable_object (id_table, url); 
      if (!par_entry)
        {
          par_entry = ThreadItem_new ();
          HTHashtable_addObject (id_table, url, par_entry);
        }	
      /* add the link from the child to the parent */
      cur_entry->parent = par_entry;
      
      /* insert it with its siblings */
      if (!par_entry->child)
        par_entry->child = cur_entry;
      else
        {
          tmp_entry = par_entry->child;
          while (tmp_entry->next)
            {
              if (tmp_entry == tmp_entry->next)
                {
                  printf ("uh oh loop with tmp_entry");
                  break;
                }
              tmp_entry = tmp_entry->next;
            }
          tmp_entry->next = cur_entry;
        }
      list_cur = list_cur->next;
    }

  /* find the root set */
  keys = HTHashtable_keys (id_table);
  for (i = 0; i < HTArray_size (keys); i++)
    {
      tmp_entry = (Container *) HTHashtable_object (id_table, (char *) HTArray_data (keys)[i]);
      if (tmp_entry->parent == NULL)
        {
          if (tmp_entry == root)
            continue;
          else if (tmp_entry->object || tmp_entry->child)
            /* promote to parent */
            Container_promoteEntry (&root, tmp_entry);
          else if (tmp_entry->child == NULL)
            /* destroy */
            {
              tmp_entry->next = NULL;
              ThreadItem_delete (tmp_entry);
            }
        }
    }
  for (i = 0; i< HTArray_size (keys); i++)
    HT_FREE (HTArray_data(keys)[i]);

  HTArray_delete (keys);
  HTHashtable_delete(id_table);

  /* prune the empty containers */
  tmp_entry = root;
  while (tmp_entry)
    {
      Container_prune (&tmp_entry->child);
      tmp_entry = tmp_entry->next;
    }

  /* all is sorted by reply to, now sort it according to dates */

  tmp_entry = root;
  while (tmp_entry)
    {
      BM_recursiveSort (tmp_entry);
      tmp_entry = tmp_entry->next;
    }

  /* mark the root orphan annotations as such. After all the above sorts,
     these are all the siblings of the first container */
  /* @@ JK: is this needed for bookmarks? */
  /* AnnotThread_markOrphan (root); */

  /* copy the result to annot_list. The children of root are live replies.
     The brothers of root have messages that have lost their in-reply-to parents */
  list_tmp = NULL;
  tmp_entry = root;
  while (tmp_entry)
    {
      ConvertContainerToList (&list_tmp, tmp_entry->child);
      tmp_entry = tmp_entry->next;
    }
  List_delAll (bookmark_list, NULL);
  *bookmark_list = list_tmp;
#endif /* BOOKMARKS */
}

/*------------------------------------------------------------
  Annot_isSameURL
  Returns TRUE if both URLs are the same, FALSE otherwise.
  ------------------------------------------------------------*/
ThotBool Annot_isSameURL (char *url1, char *url2)
{
  char *tmp_url1;
  char *tmp_url2;
  ThotBool result;

  if (!url1 || !url2)
    return FALSE;

  if (!IsW3Path (url1) && !IsFilePath (url1))
    tmp_url1 = ANNOT_MakeFileURL (url1);
  else
    tmp_url1 = url1;

  if (!IsW3Path (url2) && !IsFilePath (url2))
    tmp_url2 = ANNOT_MakeFileURL (url2);
  else
    tmp_url2 = url2;

  if (!strcasecmp (tmp_url1, tmp_url2))
    result = TRUE;
  else
    result = FALSE;

  if (tmp_url1 != url1)
    TtaFreeMemory (tmp_url1);

  if (tmp_url2 != url2)
    TtaFreeMemory (tmp_url2);
  
  return (result);
}

/*------------------------------------------------------------
  AnnotThread_link2ThreadDoc
  If the document is an annotation and if this aannotation is
  part of a thread, we link the annotation's metadata with the 
  thread.
  Returns TRUE if it could do the link, FALSE otherwise.
  ------------------------------------------------------------*/
ThotBool AnnotThread_link2ThreadDoc (Document doc)
{
  ThotBool result = FALSE;
#ifdef ANNOT_ON_ANNOT
  Document thread_doc;

  /* @@ JK: if doc is of type annot, search for an annotation for its
     url. If we found one and it is of type inreplyto, then init the
     DocumentMetaData.thread for this annotation to point to the source
     of the thread */
  if (DocumentTypes[doc] == docAnnot && AnnotMetaData[doc].thread == NULL)
    {
      thread_doc = AnnotThread_searchThreadDoc (DocumentURLs[doc]);
      if (thread_doc > 0)
        {
          AnnotMetaData[doc].thread = &AnnotThread[thread_doc];
          result = TRUE;
        }
    }
#endif /*ANNOT_ON_ANNOT*/
  return result;
}

/* ------------------------------------------------------------
   AnnotMeta_new
   Creates a new annotation metadata element
   ------------------------------------------------------------*/
AnnotMeta *AnnotMeta_new (void)
{
  AnnotMeta *new_;

  new_ = (AnnotMeta *) malloc (sizeof (AnnotMeta));
  if (new_)
    memset (new_, 0, sizeof (AnnotMeta));
  return new_;
}

/* ------------------------------------------------------------
   AnnotFilter_free
   Frees a linked list of annotation filters.
   ------------------------------------------------------------*/
void AnnotFilter_free (List *annot_list, ThotBool (*del_function)(void *))
{
  List *list_ptr, *next;
  AnnotFilterData *filter;

  list_ptr = annot_list;
  while (list_ptr)
    {
      filter = (AnnotFilterData *) list_ptr->object;
      if (del_function && filter->object)
        (*del_function) (filter->object);
      TtaFreeMemory (filter);
      next = list_ptr->next;
      TtaFreeMemory (list_ptr);
      list_ptr = next;
    }
}

/* ------------------------------------------------------------
   AnnotList_free
   Frees a linked list of annotations.
   ------------------------------------------------------------*/
void AnnotList_free (List *annot_list)
{
  List *list_ptr, *next;

  list_ptr = annot_list;
  while (list_ptr)
    {
      Annot_free ((AnnotMeta *) list_ptr->object);
      next = list_ptr->next;
      free (list_ptr);
      list_ptr = next;
    }
}

/* ------------------------------------------------------------
   Annot_free
   Frees a single annotation.
   ------------------------------------------------------------*/
void Annot_free (AnnotMeta *annot)
{
  if (annot->title)
    free (annot->title);
  if (annot->annot_url)
    free (annot->annot_url);
  if (annot->source_url) 
    free (annot->source_url);
  if (annot->author) 
    free (annot->author);
  if (annot->xptr)
    free (annot->xptr);
  if (annot->cdate) 
    free (annot->cdate);
  if (annot->mdate) 
    free (annot->mdate);
  if (annot->content_type) 
    free (annot->content_type);
  if (annot->content_length) 
    free (annot->content_length);
  if (annot->body_url) 
    free (annot->body_url);
  if (annot->body) 
    free (annot->body);
  if (annot->name)
    free (annot->name);
#ifdef ANNOT_ON_ANNOT
  if (annot->rootOfThread)
    free (annot->rootOfThread);
  if (annot->inReplyTo)
    free (annot->inReplyTo);
#endif
  free (annot);
}
 
/* ------------------------------------------------------------
   AnnotList_print
   Prints the contents For each element of a linked list of 
   annotations metadata.
   ------------------------------------------------------------*/
void AnnotList_print (List *annot_list)
{
  AnnotMeta *annot;
  List *annot_ptr;

  annot_ptr = annot_list;
  while (annot_ptr)
    {
      annot = (AnnotMeta *) annot_ptr->object;
      printf("\n=====annotation meta data =========\n");  
      if (annot->source_url)
        printf ("annot source URL = %s\n", annot->source_url);
      if (annot->type)
        printf ("annot type = %s\n", annot->type->name);
      if (annot->xptr)
        printf ("annot context = %s\n", annot->xptr);
      if (annot->labf)
        printf ("annot labf = %s, c1 = %d\n", annot->labf, annot->c1);
      if (annot->labl)
        printf ("annot labl = %s, cl = %d\n", annot->labl, annot->cl);
      if (annot->author) 
        printf ("author is = %s\n", annot->author);
      if (annot->content_type)
        printf ("content_type is = %s\n", annot->content_type);
      if (annot->content_length) 
        printf ("content_length is = %s\n", annot->content_length);
      if (annot->body_url)
        printf ("body url is = %s\n", annot->body_url);
      if (annot->body)
        printf ("======= body =============\n%s", annot->body);
      printf ("=========================\n");
      annot_ptr = annot_ptr->next;
    }
  printf ("\n");
}

/* ------------------------------------------------------------
   Annot_dumpCommonMeta
   Dumps the common metatada for both local and remote annotations
   to the file pointed by fp.
   ------------------------------------------------------------*/
static void  Annot_dumpCommonMeta (AnnotMeta *annot, FILE *fp)
{
  char *tmp;
#ifdef ANNOT_ON_ANNOT
  Document source_doc;
  Document annot_doc;
#endif /* ANNOT_ON_ANNOT */

#ifdef ANNOT_ON_ANNOT
  if (annot->inReplyTo)
    {
      fprintf (fp, 
               "<r:type r:resource=\"%s\" />\n", THREAD_NS THREAD_REPLY_LOCAL_NAME);
    }
  else
#endif /* ANNOT_ON_ANNOT */
    fprintf (fp, 
             "<r:type r:resource=\"%s\" />\n", ANNOTATION_CLASSNAME);
  
  if (annot->type && annot->type != ANNOTATION_CLASS)
    fprintf (fp, 
             "<r:type r:resource=\"%s\" />\n",
             annot->type->name);
  
#ifdef ANNOT_ON_ANNOT
  /* replies don't have the annotates property */
  if (!annot->inReplyTo)
#endif /* ANNOT_ON_ANNOT */
    fprintf (fp, 
             "<a:annotates r:resource=\"%s\" />\n",
             annot->source_url);
  
  /* @@ JK: Removed because we're now using xptr */
#if 0
  fprintf (fp,
           "<a:context>#id(%s|%d|%s|%d)</a:context>\n",
           annot->labf,
           annot->c1,
           annot->labl,
           annot->cl);
#endif
  
#ifdef ANNOT_ON_ANNOT
  /* JK: A patch so that the context points to the body and not elsewhere */
  annot_doc = Annot_searchDocument (annot->body_url);
  if (!annot->annot_url)
    annot->annot_url = annot->body_url;
  source_doc = Annot_IsDocumentLoaded (annot_doc, annot->body_url, NULL);
  if (annot->annot_url == annot->body_url)
    annot->annot_url = NULL;
  if (source_doc != 0 && DocumentTypes[source_doc] == docAnnot)
    fprintf (fp,
             "<a:context>%s#%s</a:context>\n",
             DocumentURLs[source_doc],
             annot->xptr);
  else
#endif /* ANNOT_ON_ANNOT */
    fprintf (fp,
             "<a:context>%s#%s</a:context>\n",
             annot->source_url,
             annot->xptr);
  
  if (annot->title)
    {
      tmp = annot->title;
      fprintf (fp,
               "<d:title>%s</d:title>\n",
               tmp);
    }
  
  if (annot->author)
    {
      tmp = annot->author;
      fprintf (fp,
               "<d:creator>%s</d:creator>\n",
               tmp);
    }

  fprintf (fp,
           "<a:created>%s</a:created>\n",
           annot->cdate);
  
  fprintf (fp,
           "<d:date>%s</d:date>\n",
           annot->mdate);
  
#ifdef ANNOT_ON_ANNOT
  if (annot->inReplyTo)
    {
      fprintf (fp,
               "<t:root r:resource=\"%s\" />\n",
               annot->rootOfThread);
      fprintf (fp,
               "<t:inReplyTo r:resource=\"%s\" />\n",
               annot->inReplyTo);
    }
#endif /* ANNOT_ON_ANNOT */
}

/* ------------------------------------------------------------
   AnnotList_dumpList
   Dumps a list of annotations to the file pointed to by
   fp. fp must be opened and closed outside of this function.
   ------------------------------------------------------------*/
static void  Annot_dumpList (List *annot_list, FILE *fp)
{
  List *annot_ptr;
  AnnotMeta *annot;

  /*write each annotation entry */
  annot_ptr = annot_list;
  while (annot_ptr)
    {
      annot = (AnnotMeta *) annot_ptr->object;
     
      /* only save the local annotations; these either have a NULL URI
         (if they were newly created this session) or have a file: URI */
      if (!annot->annot_url || IsFilePath(annot->annot_url))
        {
          fprintf (fp, 
                   "<r:Description>\n");
          Annot_dumpCommonMeta (annot, fp);
          fprintf (fp,
                   "<a:body r:resource=\"%s\" />\n",
                   annot->body_url);
	  
          fprintf (fp, 
                   "</r:Description>\n");
        }
      annot_ptr = annot_ptr->next;
    }
}

/* ------------------------------------------------------------
   AnnotList_writeIndex
   Writes an RDF annotation index file from the contents
   of annot_list.
   ------------------------------------------------------------*/
void AnnotList_writeIndex (char *indexFile, List *annot_list, 
                           List *thread_list)
{
  FILE *fp;

  if (!(annot_list || thread_list) || !indexFile || indexFile[0] == EOS)
    return;

  fp = TtaWriteOpen (indexFile);
  /* write the prologue */
  fprintf (fp,
           "<?xml version=\"1.0\" ?>\n" 
           "<r:RDF xmlns:r=\"" RDFMS_NS "\"\n"
           "xmlns:a=\"%s\"\n"
#ifdef ANNOT_ON_ANNOT
           "xmlns:t=\"" THREAD_NS "\"\n"
#endif /* ANNOT_ON_ANNOT */
           "xmlns:http=\"" HTTP_NS "\"\n"
           "xmlns:d=\"" DC_NS "\">\n",
           ANNOT_NS);
  
  /* dump the standard annotations */
  Annot_dumpList (annot_list, fp);

#ifdef ANNOT_ON_ANNOT
  /* dump the replies (thread) */
  Annot_dumpList (thread_list, fp);
#endif /* ANNOT_ON_ANNOT */

  /* write the epiloge */
  fprintf (fp, "</r:RDF>\n");
  TtaWriteClose (fp);
}

/* ------------------------------------------------------------
   AnnotList_preparePostBody
   Writes an RDF file made from an annotations metadata
   and the annotations HTML body. 
   It returns the name of the RDF file ($APP_TMPDIR/rdf.tmp)
   ------------------------------------------------------------*/
char * ANNOT_PreparePostBody (Document doc)
{
  FILE *fp, *fp2;
  char tmp_str[80];
  char *rdf_tmpfile, *ptr, *ret;
  char *html_tmpfile;
  const char *content_type;
  ThotBool new_annotation;

  AnnotMeta *annot;
  unsigned long content_length;

  /* we get the metadata associated to DocAnnot */
  annot = GetMetaData (DocumentMeta[doc]->source_doc, doc);

  if (!annot)
    return FALSE;

  /* compute the temporary file names */
  ptr = TtaGetEnvString ("APP_TMPDIR");
  rdf_tmpfile = (char *)TtaGetMemory (strlen (ptr) + sizeof ("rdf.tmp") + 2);
  sprintf (rdf_tmpfile, "%s%c%s", ptr, DIR_SEP, "rdf.tmp");
  html_tmpfile = (char *)TtaGetMemory (strlen (ptr) + sizeof ("html.tmp") + 2);
  sprintf (html_tmpfile, "%s%c%s", ptr, DIR_SEP, "html.tmp");

  /* output the HTML body */
  ANNOT_LocalSave (doc, html_tmpfile);

  /* find the size of the html fragment */
  AM_GetFileSize (html_tmpfile, &content_length);

  fp = TtaWriteOpen (rdf_tmpfile);
  /* write the prologue */
  fprintf (fp,
           "<?xml version=\"1.0\" ?>\n" 
           "<r:RDF xmlns:r=\"" RDFMS_NS "\"\n"
           "xmlns:a=\"%s\"\n"
#ifdef ANNOT_ON_ANNOT
           /* @@@ jk: add an if so that we only add this NS 
              when dumping a reply */
           "xmlns:t=\"" THREAD_NS "\"\n"
#endif /* ANNOT_ON_ANNOT */
           "xmlns:http=\"" HTTP_NS "\"\n"
           "xmlns:d=\"" DC_NS "\">\n",
           ANNOT_NS);

  /* beginning of the annotation's  metadata  */
  if (IsW3Path (DocumentURLs[doc]))
    {
      /* we're saving a modification to an existing annotation */
      fprintf (fp, "<r:Description r:about=\"%s\">\n", annot->annot_url);
      new_annotation = FALSE;
    }
  else
    {
      /* it's a new annotation */
      fprintf (fp, "<r:Description>\n");
      new_annotation = TRUE;
    }

  /* dump the common metadata */
  Annot_dumpCommonMeta (annot, fp);

  /* the body of the annotation prologue */
  content_type = (DocumentMeta[doc]->content_type) 
    ? DocumentMeta[doc]->content_type : AM_XHTML_MIME_TYPE;
  fprintf (fp, "<a:body>\n");
  if (new_annotation)
    {
      fprintf (fp,
               "<r:Description>\n");
    }
  else
    {
      fprintf (fp,
               "<r:Description r:about=\"%s\">\n",
               DocumentURLs[doc]);
    }

  fprintf (fp,
           "<http:ContentType>%s</http:ContentType>\n"
           "<http:ContentLength>%ld</http:ContentLength>\n"
           "<http:Body r:parseType=\"Literal\">\n",
           content_type,
           content_length);

  /* 
  ** insert the HTML body itself
  */

  fp2 = TtaReadOpen (html_tmpfile);
  if (fp2)
    {
      /* skip any prologue (to have a valid XML doc )*/
      while (!feof(fp2))
        {
          ret = fgets (tmp_str, 79, fp2);
          if (strncmp(tmp_str, "<h", 2) == 0) /* looking for <html... */
            {
              break;
            }
        }
      while (!feof (fp2))
        {
          fprintf (fp, "  %s", tmp_str);
          ret = fgets (tmp_str, 79, fp2);
        }
      TtaReadClose (fp2);
    }

  TtaFileUnlink (html_tmpfile);
  TtaFreeMemory (html_tmpfile);

  /* finish writing the annotation */
  fprintf (fp, 
           "</http:Body>\n"
           "</r:Description>\n"
           "</a:body>\n"
           "</r:Description>\n"
           "</r:RDF>\n");

  TtaWriteClose (fp);  
  return (rdf_tmpfile);
}

/* ------------------------------------------------------------
   StrDupDate
   Returns a pointer to a memalloc'd string containing the current date.
   It's up to the caller to free this memory.
   ------------------------------------------------------------*/
char *StrdupDate (void)
{
  time_t      curDate;
  struct tm   *localDate;
  char      *strDate;
  int        UTCoffset;
  int        UTChourOffset;
  int        UTCminOffset;
  
  curDate = time (&curDate);
  localDate = localtime (&curDate);

#ifdef _WINGUI
  UTCoffset = _timezone;	/* global, set by localtime() */
#else
  UTCoffset = (int)timezone;		/* global, set by localtime() */
#endif /* _WINGUI */

  UTChourOffset = UTCminOffset = abs(UTCoffset)/60; /* UTCoffset is seconds */
  UTChourOffset /= 60;
  UTCminOffset %= 60;

  /* @@ possible memory bug */
  strDate = (char *)TtaGetMemory (26);
  sprintf (strDate,
           "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
           localDate->tm_year+1900,
           localDate->tm_mon+1,
           localDate->tm_mday, 
           localDate->tm_hour,
           localDate->tm_min,
           localDate->tm_sec,
           (UTCoffset > 0) ? '-' : '+',
           UTChourOffset,
           UTCminOffset);
  return (strDate);
}

/* ------------------------------------------------------------
   StrDateToCalTime
   Converts a string date of form: (see http://www.w3.org/TR/NOTE-datetime)
   YYYY-MM-DDTHH:MM[:SS[.S]]
   YYYY-MM-DDTHH:MM[:SS[.S]]Z
   YYYY-MM-DDTHH:MM[:SS[.S]]+HH:MM
   YYYY-MM-DDTHH:MM[:SS[.S]]-HH:MM
   into calendar time represenation (that is, number of seconds
   since the Unix epoch).  At most 12 to 14 fractional digital are ignored.

   The first case (no time zone designator) is for backwards
   compatibility only; it is assumed to be in the local timezone.
   ------------------------------------------------------------*/
time_t StrDateToCalTime (char *strDate)
{
  struct tm scanLocal;
  time_t cal_date;
  char tzdata[20];
  char *tzdataP;
  int UTCoffset = 1;
  int UTChourOffset = 0;
  int UTCminuteOffset = 0;

  memset (&scanLocal, 0, sizeof (scanLocal));
  memset (tzdata, 0, sizeof (tzdata));

  sscanf (strDate, "%04d-%02d-%02dT%02d:%02d%19s",
          &scanLocal.tm_year,
          &scanLocal.tm_mon,
          &scanLocal.tm_mday,
          &scanLocal.tm_hour,
          &scanLocal.tm_min,
          tzdata);

  scanLocal.tm_year -= 1900;
  scanLocal.tm_mon--;

  tzdataP = &tzdata[0];
  if (*tzdataP == ':') {
    if (sscanf (tzdataP, ":%02d", &scanLocal.tm_sec)) {
      tzdataP += 3;
    }
  }

  if (*tzdataP == '.') {	/* ignore fractional seconds */
    tzdataP++;
    while (*tzdataP >= '0' && *tzdataP <= '9') {tzdataP++;}
  }

  cal_date = mktime(&scanLocal);

  /* mktime has already included the UTC offset for that date;
     if a time zone designator was specified, we must drop out the
     UTC offset and add the designated offset */

  switch (*tzdataP)
    {
    case 0:			/* no timezone designator */
      break;

    case '+':			/* a designated offset */
      UTCoffset = -1;		/* UTC is behind local time */
      /* fall through */

    case '-':			/* UTC is ahead of local time */
      /* UTCoffset was initialized to +1 */
      tzdataP++;
      sscanf (tzdataP, "%02d:%02d",
              &UTChourOffset,
              &UTCminuteOffset);
      cal_date += UTCoffset * ((UTChourOffset * 60) + UTCminuteOffset) * 60;
      /* fall through */

    case 'Z':			/* subtract mktime()'s UTC offset */
    case 'z':
      (void)localtime( &cal_date ); /* set timezone global */

#ifdef _WINGUI
      UTCoffset = _timezone;	/* global, set by localtime() */
#else
      UTCoffset = (int)timezone;	/* global, set by localtime() */
#endif /* _WINGUI */

      cal_date -= UTCoffset;
      break;
    }

  return (cal_date);
}

/***************************************************
 **
 **************************************************/

/*-----------------------------------------------------------------------
  SearchAnnotation
  Searches doc and returns the link element that points to annotDoc, or
  NULL if it doesn't exist.
  -----------------------------------------------------------------------*/
Element SearchAnnotation (Document doc, char *annotDoc)
{
  ElementType elType;
  Element     elCour;
  char       *ancName = NULL;

  elType.ElSSchema = TtaGetSSchema ("XLink", doc);
  if (!elType.ElSSchema)
    /* there are no XLink annotations in this document! */
    return NULL;
  elType.ElTypeNum = XLink_EL_XLink;

  /* Search the anchor that points to the annotDoc */
  elCour = TtaSearchTypedElement (elType, SearchInTree, 
                                  TtaGetRootElement (doc));
  while (elCour != NULL) 
    {
      ancName = SearchAttributeInEl (doc, elCour, XLink_ATTR_id, "XLink");
      if (ancName) 
        {
          if (!strcmp (ancName, annotDoc))
            break;
          TtaFreeMemory (ancName);
          ancName = NULL;
        }
      /* @@ JK: do we need to get the succesor? */
      /* @@ RRS: apparently not.  TtaSearchTypedElement seems to start
         with the successor.  When two annot (XLink) elements are in
         a row, the GetSuccessor caused the second one to be skipped. */
      /* elCour = TtaGetSuccessor (elCour); */
      elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
    }

  if (ancName)
    TtaFreeMemory (ancName);

  return elCour;
}

/*-----------------------------------------------------------------------
  ReplaceLinkToAnnotation
  Changes the annotation link pointing to oldAnnotURL so that it'll point
  to newAnnotURL.
  If no oldAnnotURL link is found, returns FALSE, otherwise returns TRUE.
  -----------------------------------------------------------------------*/
ThotBool ReplaceLinkToAnnotation (Document doc, char *annotName,
                                  char *newAnnotURL)
{
  Element anchor;
  ElementType elType;

  AttributeType  attrType;
  Attribute      attr;
  
  anchor = SearchAnnotation (doc, annotName);
  if (!anchor)
    return FALSE;

  elType = TtaGetElementType (anchor);
  attrType.AttrSSchema = GetXLinkSSchema (doc);
  attrType.AttrTypeNum = XLink_ATTR_href_;
  attr = TtaGetAttribute (anchor, attrType);
  if (!attr)
    return FALSE;
  TtaSetAttributeText (attr, newAnnotURL, anchor, doc);

  return TRUE;
}

/*-----------------------------------------------------------------------
  SearchElementInDoc
  Returns the first element of type elTypeNum found in the document
  or NULL if it doesn't exist.
  -----------------------------------------------------------------------*/
Element SearchElementInDoc (Document doc, int elTypeNum)
{
  Element     root;
  ElementType elType;

  if ((root = TtaGetRootElement (doc)) == NULL)
    return NULL;
  elType = TtaGetElementType (root);
  elType.ElTypeNum = elTypeNum;
  return TtaSearchTypedElement (elType, SearchInTree, root);
}

/*-----------------------------------------------------------------------
  SearchAttributeInEl (doc, el, attrTypeNum, schema)
  Returns the value of attribute type attrTypeNum if it exists in the
  document element or NULL otherwise.
  -----------------------------------------------------------------------*/
char *SearchAttributeInEl (Document doc, Element el, int attrTypeNum, 
                           const char *schema)
{
  AttributeType  attrType;
  Attribute      attr;
  char          *text;
  int            length;

  if (!el) 
    return NULL;

  attrType.AttrSSchema = TtaGetSSchema (schema, doc);
  attrType.AttrTypeNum = attrTypeNum;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return NULL;
  else
    {
      length = TtaGetTextAttributeLength (attr);
      text = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, text, &length);
      return text;
    }
}

/*-----------------------------------------------------------------------
  SubstituteCharInString
  Substitutes all occurences of old char with new char in string buffer
  -----------------------------------------------------------------------*/
void SubstituteCharInString (char *buffer, char old, char new_)
{
  char *ptr;

  ptr = buffer;
  while (*ptr)
    {
      if (*ptr == old)
        *ptr = new_;
      ptr++;
    }
}

#if 0
/*-----------------------------------------------------------------------
  GetTempName
  Front end to the Unix tempnam function, which is independent of the
  value of the TMPDIR env value 
  Returns a dynamically allocated string with a tempname. The user
  must free this memory.
  -----------------------------------------------------------------------*/
char *GetTempName (const char *dir, const char *prefix)
{
  char *tmpdir;
  char *tmp;
  char *name = NULL;

  /* save the value of TMPDIR */
  tmp = getenv (TMPDIR);

  if (tmp)
    {
      tmpdir = TtaStrdup (tmp);
    }
  else
    tmpdir = NULL;

  /* remove TMPDIR from the environment */
  if (tmpdir)
    {
      tmp = TtaGetMemory (strlen (tmpdir) + 2);
      sprintf (tmp, "%s=", TMPDIR);
#ifdef _WINGUI
      _putenv (tmp);
#else
      putenv (tmp);
#endif /* _WINGUI */
    }

  /* create the tempname */
#ifdef _WINGUI
  /* Under Windows, _tempnam returns the same name until the file is created */
  {
    char *altprefix;
    name = tmpnam (NULL);	/* get a possibly unique string */
    altprefix = TtaGetMemory(strlen (prefix) + strlen(name) + 1);
    sprintf (altprefix, "%s%s", prefix, name + strlen(_P_tmpdir));
    name = _tempnam (dir, altprefix); /* get a name that isn't yet in use */
    TtaFreeMemory (altprefix);
  }
#else
  name = tempnam (dir, prefix);
#endif /* _WINGUI */

  /* restore the value of TMPDIR */
  if (tmpdir)
    {
#ifdef _WINGUI
      _putenv (tmpdir);
#else
      putenv (tmpdir);
#endif /* _WINGUI */
      /* Shouldn't be free (see man for putenv ()) */
      /* TtaFreeMemory (tmpdir); */
    }
  return (name);
}
#endif

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void GetServerName (char *url, char *server)
{
  char      *scratch_url;
  char      *protocol;
  char      *host;
  char      *dir;
  char      *file;

  if (!url || IsFilePath (url))
    strcpy (server, "localhost");
  else
    {
      scratch_url = TtaStrdup (url);
      ExplodeURL (scratch_url, &protocol, &host, &dir, &file);
      strcpy (server, host ? host : "?");
      if (dir && dir[0])
        {
          strcat (server, "/");
          strcat (server, dir);
        }
      TtaFreeMemory (scratch_url);
      /* remove the query string */
      scratch_url = strrchr (server, '?');
      if (scratch_url)
        *scratch_url = EOS;
    }
}

/*-----------------------------------------------------------------------
  ANNOT_GetHTMLTitle
  If doc is of type (X)HTML, returns the HTML title if it's not empty.
  If doc is of type Annotation, returns the title of the annotation if
  it's not empty.
  Otherwise, returns the URL of the document.
  The caller must free the returned string.
  -----------------------------------------------------------------------*/
char * ANNOT_GetHTMLTitle (Document doc)
{
  Element          el;
  int              length;
  Language         lang;
  char            *title;
  ElementType      elType;

  title = NULL;
  if (DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docXml)
    {
      /* only HTML documents and XHTML documents have a title */
      /* @@ JK: should get the XHTML or HTML doc schema here */
      /* find the title */
      el = TtaGetRootElement (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_TITLE;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      /* found a title */
      if (el)
        {
          /* find the text content */
          el = TtaGetLastChild (el);
          /* with some content */
          if (el)
            {
              length = TtaGetTextLength (el) + 1;
              if (length > 1)
                {
                  title = (char *)TtaGetMemory (length);
                  TtaGiveTextContent (el, (unsigned char *)title, &length, &lang);
                  /* discard an empty title */
                  if (title[0] == EOS)
                    {
                      TtaFreeMemory (title);
                      title = NULL;
                    }
                }
            }
        }
    }
  else if (DocumentTypes[doc] == docAnnot)
    {
      elType.ElSSchema = TtaGetDocumentSSchema (doc);
      elType.ElTypeNum = Annot_EL_ATitle;
      /* find the title */
      el = TtaGetRootElement (doc);
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      /* found a title */
      if (el)
        {
          /* find the text content */
          el = TtaGetLastChild (el);
          /* with some content */
          if (el)
            {
              length = TtaGetTextLength (el) + 1;
              if (length > 1)
                {
                  title = (char *)TtaGetMemory (length);
                  TtaGiveTextContent (el, (unsigned char *)title, &length, &lang);
                  /* discard an empty title */
                  if (title[0] == EOS)
                    {
                      TtaFreeMemory (title);
                      title = NULL;
                    }
                }
            }
        }
    }
  /* if we didn't get a title, use the document's URL instead */
  if (title == NULL)
    title = TtaStrdup (DocumentURLs[doc]);
  return (title);
}

/*-----------------------------------------------------------------------
  ANNOT_SetType
  Sets the annotation type of annotation document doc to the value of
  type, if type is not empty.
  -----------------------------------------------------------------------*/
void ANNOT_SetType (Document doc, RDFResourceP type)
{
  Element          el;
  ElementType      elType;
  char          *url;
  int              i;
  AnnotMeta       *annot;
  char          *type_name;
  ThotBool       in_thread = FALSE;

  if (!type)
    return;

  /* only HTML documents can be annotated */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = Annot_EL_RDFtype;
  /* find the type */
  el = TtaGetRootElement (doc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  /* no such element */
  if (!el)
    return;
  /* change the text content */
  el = TtaGetFirstChild (el);
  type_name = ANNOT_GetLabel(&annot_schema_list, type);
  TtaSetTextContent (el, (unsigned char *)type_name,
                     TtaGetDefaultLanguage (), doc);

  /* update the metadata */
  if (!IsFilePath (DocumentURLs[doc])
      && !IsW3Path (DocumentURLs[doc]))
    url = ANNOT_MakeFileURL (DocumentURLs[doc]);
  else
    url = DocumentURLs[doc];

  for (i = 1; i <= DocumentTableLength; i++)
    {
      /* find the annotation link in the source document that corresponds
         to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[i].annotations,
                                     url, AM_BODY_URL);
#ifdef ANNOT_ON_ANNOT
      if (!annot && AnnotMetaData[i].thread)
        {
          annot = AnnotList_searchAnnot (AnnotMetaData[i].thread->annotations,
                                         url, AM_BODY_URL);
          if (annot)
            in_thread = TRUE;
        }
#endif /* ANNOT_ON_ANNOT */
      if (annot)
        break;
    }

  if (annot)
    {
      annot->type = type;
#ifdef ANNOT_ON_ANNOT
      if (in_thread)
        /* update the reply type in the thread */
        ANNOT_UpdateThreadItem (doc, annot, annot->body_url);
#endif /* ANNOT_ON_ANNOT */
    }
  if (url != DocumentURLs[doc])
    TtaFreeMemory (url);
}

/*-----------------------------------------------------------------------
  ANNOT_MakeFileURL
  Returns a dynamically allocated string containing the directory path
  given in input, prefixed by the "file://" URL convention.
  It's up to the caller to free the returned string.
  -----------------------------------------------------------------------*/
char * ANNOT_MakeFileURL (const char *path)
{
  char *url;

  url = LocalToWWW ((char *) path);

  return (url);
#if 0
  /* @@ add the file:// (why it wasn't there before? */
  url = TtaGetMemory (strlen (path)
                      + sizeof ("file://")
                      + 1);
  if (url)
    sprintf (url, "file://%s", path);
  return url;
#endif
}

/*-----------------------------------------------------------------------
  ANNOT_CanAnnotate
  Returns TRUE if it's possible to annotate a given document, FALSE
  otherwise.
  -----------------------------------------------------------------------*/
ThotBool ANNOT_CanAnnotate (Document doc)
{
  DocumentType docType;
  ThotBool result;

  docType = DocumentTypes[doc];

  switch (docType) 
    {
    //case docText:
    case docImage:
    case docSource:
    case docCSS:
    case docLog:
#ifndef ANNOT_ON_ANNOT
    case docAnnot:
#endif
      result = FALSE;
      break;

    default:
      result = TRUE;
      break;
    }

  return result;
}

/*-----------------------------------------------------------------------
  ANNOT_GetHTMLRoot
  Returns the root element of the HTML body of an annotation document, NULL
  if it's not an annotation document.
  If getFirstChild is FALSE, we return the element containing the Annot
  BODY. Otherwise, we return the first child of this element (the real
  HTML root).
  -----------------------------------------------------------------------*/
Element ANNOT_GetHTMLRoot (Document doc, ThotBool getFirstChild)
{
  ElementType elType;
  Element el;

  /* we search the start of HTML document in the annotation struct */
  if (DocumentTypes[doc] == docAnnot)
    {
      el = TtaGetRootElement (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = Annot_EL_Body;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      if (getFirstChild)
        el = TtaGetFirstChild (el);
    }
  else 
    el = NULL;
  
  return (el);
}

/*-----------------------------------------------------------------------
  ANNOT_GetBodySSchema
  Returns the S Schema associated with the body.
  -----------------------------------------------------------------------*/
SSchema ANNOT_GetBodySSchema (Document doc)
{
  ElementType elType;
  Element     el;
  SSchema     result = NULL;

  el = ANNOT_GetHTMLRoot (doc, TRUE);

  if (el)
    {
      elType = TtaGetElementType (el);
      result = elType.ElSSchema;
    }

  return result;
}

/*-----------------------------------------------------------------------
  ANNOT_GetBodySSchemaName
  Returns the S Schema name associated with the body.
  -----------------------------------------------------------------------*/
char * ANNOT_GetBodySSchemaName (Document doc)
{
  ElementType elType;
  Element     el;
  char       *result = NULL;

  el = ANNOT_GetHTMLRoot (doc, TRUE);

  if (el)
    {
      elType = TtaGetElementType (el);
      result = TtaGetSSchemaName (elType.ElSSchema);
    }

  return result;
}

/*-----------------------------------------------------------------------
  ANNOT_CreateBodyTree
  Adds the first HTML element to the annotation body.
  -----------------------------------------------------------------------*/
void ANNOT_CreateBodyTree (Document doc, DocumentType bodyType)
{
  ElementType elType;
  Element     body, el;
  ThotBool    result;
  ThotBool    oldStructureChecking;
  const char *ns_uri = NULL;

  if (DocumentTypes[doc] == docAnnot)
    {
      el = TtaGetRootElement (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = Annot_EL_Body;
      body = TtaSearchTypedElement (elType, SearchInTree, el);
      /* body = TtaGetLastChild (el); */
      if (bodyType == docSVG)
        {
          elType.ElSSchema = GetSVGSSchema (doc);
          elType.ElTypeNum = SVG_EL_SVG;
          ns_uri = SVG_URI;
        }
      else if (bodyType == docMath)
        {
          elType.ElSSchema = GetMathMLSSchema (doc);
          elType.ElTypeNum = MathML_EL_MathML;
          ns_uri = MathML_URI;
        }
      else if (bodyType == docText)
        {
          elType.ElSSchema =  GetTextSSchema (doc);
          elType.ElTypeNum = TextFile_EL_TextFile;
        }
      else if (bodyType == docXml)
        {
          elType.ElSSchema = GetGenericXMLSSchema ("XML", doc);
          elType.ElTypeNum = XML_EL_XML;
        }
      else /* consider the default case to be HTML */
        {
          elType.ElSSchema = GetXHTMLSSchema (doc);
          elType.ElTypeNum = HTML_EL_HTML;
          ns_uri = XHTML_URI;
        }
      el = TtaNewTree (doc, elType, "");
      result = TtaCanInsertFirstChild (elType, body, doc);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      TtaInsertFirstChild (&el, body, doc);
      /* set the namespace declaration */
      if (ns_uri)
        TtaSetANamespaceDeclaration (doc, el, NULL, ns_uri);
      /* set attribute dir on the Document element */
      AddDirAttributeToDocEl (doc);
      TtaSetStructureChecking (oldStructureChecking, doc);
    }
}

/*-----------------------------------------------------------------------
  WWWToLocal
  Converts a file: URL into a local file system one. Result is stored
  in the memory pointed to by the input argument.
  -----------------------------------------------------------------------*/
void WWWToLocal (char *url)
{
  char *tmp;
  char *tmp_url, *esc_url;
  char target[MAX_LENGTH];

  ThotBool free_tmp_url;

  if (!url || !IsFilePath (url))
    return;

  ExtractTarget (url, target);

  /* Because of a previous problem, some of the local annotations were
     stored with wrong URLs. That is, of type file: and with backslashes.
     If this is the case, we try to convert them to slashes before invoking
     the libwww function. Otherwise, the convertion will fail.
  */
  if (strchr (url, '\\'))
    {
      tmp_url = TtaStrdup (url);
      free_tmp_url = TRUE;
      tmp = tmp_url;
      while (*tmp)
        {
          if (*tmp == '\\')
            *tmp = '/';
          tmp++;
        }
    }
  else
    {
      tmp_url = url;
      free_tmp_url = FALSE;
    }

  /* @@ IV: Some of the local annotations could include spaces.
     If this is the case, we convert them to %20 before invoking
     the libwww function.
  */
  esc_url = EscapeURL (tmp_url);
  tmp = HTWWWToLocal (esc_url, "file://", NULL);
  if (free_tmp_url)
    TtaFreeMemory (tmp_url);
  TtaFreeMemory (esc_url);

  /* @@ JK: A patch for Windows */
  if (tmp[0] == DIR_SEP && tmp[1] == DIR_SEP && tmp[3] ==':')
    strcpy (url, tmp + 2);
  else
    strcpy (url, tmp);
  if (target[0])
    strcat (url, target);
  HT_FREE (tmp);
}

/*-----------------------------------------------------------------------
  LocalToWWW
  Converts a local URL into a file: one. Caller has to free the
  returned string.
  -----------------------------------------------------------------------*/
char * LocalToWWW (char *url)
{
  char *tmp;
  
  /* if the URL doesn't have a path, return */
  if (!url || (url[0] != DIR_SEP && url[1] != ':'))
    return NULL;

  tmp = HTLocalToWWW (url, "file://");
  return (tmp);
}

/*-----------------------------------------------------------------------
  TestLocalToWWW
  If the string that's given in input is a local URL, it
  converts it into a file: one. Caller has to free the
  returned string.
  If the string is already a file: or other kind of URL, it
  returns the same string.
  -----------------------------------------------------------------------*/
char * TestLocalToWWW (char *url)
{
  char *tmp;

  /* if the URL doesn't have a path, return */
  if (!url || (url[0] != DIR_SEP && url[1] != ':'))
    return url;

  if (IsFilePath (url) || IsW3Path (url))
    return url;

  tmp = HTLocalToWWW (url, "file://");
  return (tmp);
}

/*-----------------------------------------------------------------------
  FixFileURL
  This function makes sure that all the local URLs have the same format.
  -----------------------------------------------------------------------*/

char *FixFileURL (char *url)
{
  char *tmp, *fixed_url;

  tmp = TtaStrdup ((char *) url);
  WWWToLocal (tmp);
  fixed_url = LocalToWWW (tmp);
  TtaFreeMemory (tmp);

  return (fixed_url);
}

/*-----------------------------------------------------------------------
  Annot_IsReplyTo
  Returns true if the document is a reply to an annotation.
  -----------------------------------------------------------------------*/

ThotBool Annot_IsReplyTo (Document doc_annot)
{
#ifdef ANNOT_ON_ANNOT
  ThotBool        isReplyTo;
  char           *annot_url;
  Document        source_doc;
  AnnotMeta      *annot;

  if (DocumentTypes[doc_annot] != docAnnot)
    return FALSE;

  /* we need to know if it's a reply to an annotation early on, so that
     we can update the annotation indexes accordingly */
  source_doc = DocumentMeta[doc_annot]->source_doc;
  if (IsW3Path (DocumentURLs[doc_annot]))
    annot_url = DocumentURLs[doc_annot];
  else
    annot_url = ANNOT_MakeFileURL (DocumentURLs[doc_annot]);
  annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                 annot_url, AM_BODY_URL);

  if (!annot && AnnotMetaData[source_doc].thread)
    annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].thread->annotations,
                                   annot_url, AM_BODY_URL);

  if (annot_url != DocumentURLs[doc_annot])
    TtaFreeMemory (annot_url);
  if (annot)
    isReplyTo = annot->inReplyTo != NULL;
  else
    isReplyTo = FALSE;
  
  return isReplyTo;
#else
  return FALSE;
#endif /* ANNOT_ON_ANNOT */
}  

/*-----------------------------------------------------------------------
  Annot_IsDocumentLoaded
  Returns the document ID if a document exists with this URL
  -----------------------------------------------------------------------*/

Document Annot_IsDocumentLoaded (Document annot_doc, char *source_annot_url, char *form_data)
{
  char *annot_url;
  Document doc;
  
  if (annot_doc == 0)
    return 0;

  annot_url = NULL;
  for (doc = 1; doc < DocumentTableLength; doc++)
    {
      if (!DocumentURLs[doc] || DocumentTypes[doc] != docAnnot || doc == annot_doc)
        continue;
      annot_url = AnnotList_searchAnnotURL (doc, DocumentURLs[annot_doc]);
      if (annot_url)
        break;
    }

  if (!annot_url)
    doc = (Document) None;

  return doc;
}

/*-----------------------------------------------------------------------
  Annot_bodyType
  Returns the document type of the body of an annotation
  -----------------------------------------------------------------------*/
DocumentType ANNOT_bodyType (Document annot_doc)
{
  Document source_doc;
  AnnotMeta *annot;

  source_doc = DocumentMeta[annot_doc]->source_doc;
  annot =  GetMetaData (source_doc, annot_doc);
  if (annot)
    return (annot->bodyType);
  else
    return docHTML;
}

/*-----------------------------------------------------------------------
  Annot_bodyType_set
  Sets the document type of the body of an annotation
  -----------------------------------------------------------------------*/
void ANNOT_bodyType_set (Document annot_doc, DocumentType bodyType)
{
  Document source_doc;
  AnnotMeta *annot = NULL;
  int i;

  if (DocumentMeta[annot_doc] && DocumentMeta[annot_doc]->source_doc != 0)
    {
      source_doc = DocumentMeta[annot_doc]->source_doc;
      annot =  GetMetaData (source_doc, annot_doc);
    }
  else /* we don't know the source document, we scan everywhere to find it */
    {
      for (i=1; i< MAX_DOCUMENTS; i++)
        {
          if (i == annot_doc)
            continue;
          if (!DocumentMeta[i])
            continue;
          annot = GetMetaData (i, annot_doc);
          if (annot)
            break;
        }
    }

  if (annot)
    annot->bodyType = bodyType;
}

/*-----------------------------------------------------------------------
  Annot_SetXMLBody
  Changes the setting of the document to say that the body of the
  annotation is a document of type XML.
  -----------------------------------------------------------------------*/
void Annot_SetXMLBody (Document doc)
{
  Element        el;
  Attribute      attr;
  AttributeType  attrType;
  ThotBool       oldStructureChecking;

  if (DocumentTypes[doc] != docAnnot)
    return;

  el = TtaGetMainRoot (doc);

  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum =  Annot_ATTR_XmlBody;

  attr = TtaGetAttribute (el, attrType); 

  if (!attr)  /* attach a new attribute */
    {
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetStructureChecking (oldStructureChecking, doc);
    }
}

/*-----------------------------------------------------------------------
  Annot_DocumentURL
  Returns a string that has a copy of the document URL. If the document
  has any parameters, they are concatenated to it.
  The caller has to free the returned pointer.
  -----------------------------------------------------------------------*/
char * Annot_DocumentURL (Document doc)
{
 
  char *ptr;

  if (DocumentMeta[doc] && DocumentMeta[doc]->form_data)
    {
      int len;
      len = strlen (DocumentURLs[doc] + strlen (DocumentMeta[doc]->form_data) + 2);
      ptr = (char *)TtaGetMemory (len);
      sprintf (ptr, "%s?%s", DocumentURLs[doc], DocumentMeta[doc]->form_data);
    }
  else
    ptr = TtaStrdup (DocumentURLs[doc]);

  return ptr;
}

/*-----------------------------------------------------------------------
  Annot_ConcatenateBase
  Returns a string that concatenateshas a copy of the document URL. If the document
  has any parameters, they are concatenated to it.
  The caller has to free the returned pointer.
  -----------------------------------------------------------------------*/
char *Annot_ConcatenateBase (char *base, char *fragment)
{
  char *ptr;

  if (base && *base && fragment && *fragment)
    {
      ptr = (char *)TtaGetMemory (strlen (base) + strlen (fragment) + 1);
      sprintf (ptr, "%s%s", base, fragment);
    }
  else
    ptr = NULL;
  return (ptr);
}
