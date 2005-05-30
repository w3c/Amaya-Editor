/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTutil.c : different miscellaneous functions used by the other
 *               annotation modules
 *
 * Author: J. Kahan (W3C/ERCIM)
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

/* bookmark includes */
#include "bookmarks.h"


/*----------------------------------------------------------------------
  Bookmark_new
  ----------------------------------------------------------------------*/
BookmarkP Bookmark_new (void)
{
  BookmarkP me;

  me = (BookmarkP) TtaGetMemory (sizeof (Bookmark));
  memset (me, 0, sizeof (Bookmark));

  return (me);
}

/*----------------------------------------------------------------------
  Bookmark_free
  ----------------------------------------------------------------------*/
void Bookmark_free (BookmarkP me)
{
  if (me->parent_url)
    TtaFreeMemory (me->parent_url);
  if (me->parent_url_list)
    {
      List *list = me->parent_url_list;
      List_delAll (&list, (ThotBool (*)(void*)) TtaFreeMemory);
    }
  if (me->self_url)
    TtaFreeMemory (me->self_url);
  if (me->blank_id)
    TtaFreeMemory (me->blank_id);
  if (me->context)
    TtaFreeMemory (me->context);
  if (me->bookmarks)
    TtaFreeMemory (me->bookmarks);
  if (me->title)
    TtaFreeMemory (me->title);
  if (me->nickname)
    TtaFreeMemory (me->nickname);
  if (me->author)
    TtaFreeMemory (me->author);
  if (me->created)
    TtaFreeMemory (me->created);
  if (me->modified)
    TtaFreeMemory (me->modified);
  if (me->description)
    TtaFreeMemory (me->description);
  TtaFreeMemory (me);
}

/*------------------------------------------------------------
  Bookmark_copy
  ------------------------------------------------------------*/
BookmarkP Bookmark_copy (BookmarkP src)
{
  BookmarkP me;

  me = Bookmark_new ();
  
  if (src->parent_url)
    me->parent_url = TtaStrdup (src->parent_url);
  if (src->self_url)
    me->self_url = TtaStrdup (src->self_url);
  if (src->context)
    me->context = TtaStrdup (src->context);
  if (src->bookmarks)
    me->bookmarks = TtaStrdup (src->bookmarks);
  if (src->title)
    me->title = TtaStrdup (src->title);
  if (src->nickname)
    me->nickname = TtaStrdup (src->nickname);
  if (src->author)
    me->author = TtaStrdup (src->author);
  if (src->created)
    me->created = TtaStrdup (src->created);
  if (src->modified)
    me->modified = TtaStrdup (src->modified);
  if (src->description)
    me->description = TtaStrdup (src->description);
  me->bm_type = src->bm_type;

  return (me);
}

/*------------------------------------------------------------
  BMList_delItem
  ------------------------------------------------------------*/
ThotBool BMList_delItem (void *me)
{
  if (!me)
    return FALSE;

  Bookmark_free ((BookmarkP) me);

  return TRUE;
}

/*------------------------------------------------------------
  BMList_containsURL
  Returns true if the bookmark list has a bookmark item
  with value self_url equal to url.
  ------------------------------------------------------------*/
ThotBool BMList_containsURL (List *list, char *url)
{
  BookmarkP me;
  List *cur;
  ThotBool result = FALSE;

  if (!list || !url || *url == EOS)
    return FALSE;

  cur = list;
  while (cur)
    {
      me = (BookmarkP)cur->object;
      if (me && me->self_url && !strcasecmp (me->self_url, url))
	{
	  result = TRUE;
	  break;
	}
      cur = cur->next;
    }

  return result;
}

/*-----------------------------------------------------------
  BM_GetMDate
  ------------------------------------------------------------*/
char * BM_GetMDate (void *object)
{
  BookmarkP me;

  me = (BookmarkP) object;

  return (me->modified ? me->modified : (char *) "0000-00-00T00:00:00+00:00");
}

/*-----------------------------------------------------------
  BM_GetType
  ------------------------------------------------------------*/
ThotBool BM_IsTopic (void *object)
{
  return ((BookmarkP) object)->bm_type == BME_TOPIC;
}

/*-----------------------------------------------------------
  BM_topicIsExpanded
  ------------------------------------------------------------*/
static ThotBool BM_topicIsExpanded (List *topic_list, char *topic)
{
  List *cur;
  ThotBool result;
  BookmarkP item;

  cur = topic_list;
  result = FALSE;
  
  while (cur)
    {
      item = (BookmarkP) cur->object;
      if (item && !strcmp (item->self_url, topic))
	{
	  if (!item->collapsed)
	    result = TRUE;
	  break;
	}
      cur = cur->next;
    }

  return result;
}

/*-----------------------------------------------------------
  BM_pruneTopics
  Removes all the topics of the list whose parents
  don't exist or are collapsed
------------------------------------------------------------*/
List *BM_pruneTopics (List **topic_list, char *parent_topic_url)
{
  List *cur;
  List *new_list = NULL;
  BookmarkP item;
  
  cur = *topic_list;

  while (cur)
    {
      item = (BookmarkP) cur->object;
      /* only add items that are not collapsed or that have no parent_urls
	 (which we assimilate to root topics) or which correspond to the
	 start of a given topic */
      if (!item->parent_url && !parent_topic_url
	  || parent_topic_url && !strcmp (parent_topic_url, item->self_url)
	  || item->parent_url && BM_topicIsExpanded (*topic_list, item->parent_url))
	List_add (&new_list, (void *) item);
      else
	{
	  Bookmark_free (item);
	  cur->object = NULL;
	}
      cur = cur->next;
    }
  List_delAll (topic_list, (ThotBool (*)(void*))NULL);

  return new_list;
}

/*-----------------------------------------------------------
  BM_expandBookmarks
  if respectTopicStatus is TRUE, the bookmarks won't
  be expanded unless the corresponding topic status 
  is expanded.
------------------------------------------------------------*/
List *BM_expandBookmarks ( List **bookmark_list,
			   ThotBool respectTopicStatus,
			   List *topic_list)
{
  List *cur;
  List *cur2;
  List *new_list = NULL;
  List *parent_url_list;
  BookmarkP item;
  BookmarkP clone;
  
  cur = *bookmark_list;

  while (cur)
    {
      item = (BookmarkP) cur->object;
      parent_url_list = item->parent_url_list;
      item->parent_url_list = NULL;
      if (List_count (parent_url_list) == 1)
	{
	  item->parent_url = (char *) parent_url_list->object;
	  parent_url_list->object = NULL;
	  List_delAll (&parent_url_list, (ThotBool (*)(void*))NULL);

	  /* @@ check if this parent already exists or just skip it */
	  if (!respectTopicStatus 
	      || respectTopicStatus && BM_topicIsExpanded (topic_list, item->parent_url))
	    List_add (&new_list, (void *) item);
	  else
	    Bookmark_free (item);
	}
      else
	{
	  cur2 = parent_url_list;
	  while (cur2)
	    {
	      char *parent_url;

	      parent_url = (char *) cur2->object;
	      if (!respectTopicStatus 
		  || respectTopicStatus && BM_topicIsExpanded (topic_list, parent_url))
		{
		  /* not sure if this will work */
		  clone = Bookmark_copy (item);
		  clone->parent_url = (char *) cur2->object;
		  cur2->object = NULL;
		  /* @@ check if this parent already exists or just skip it */
		  List_add (&new_list, (void *) clone);
		}
	      cur2 = cur2->next;
	    }
	  Bookmark_free (item);
	  List_delAll (&parent_url_list, (ThotBool (*)(void*))NULL);
	}
      cur = cur->next;
    }
  List_delAll (bookmark_list, (ThotBool (*)(void*))NULL);
  return new_list;
}

#define PARAM_INCREMENT 5

/*----------------------------------------------------------------------
  BM_bufferNew
  Creates and initializes a new dynamic buffer structure.
  ----------------------------------------------------------------------*/
BM_dyn_buffer * BM_bufferNew (void)
{
  BM_dyn_buffer *me;

  me = (BM_dyn_buffer *)TtaGetMemory (sizeof (BM_dyn_buffer));
  if (me)
    {
      me->buffer = (char *)TtaGetMemory (PARAM_INCREMENT);
      me->buffer[0] = EOS;
      me->lgbuffer = PARAM_INCREMENT;
    }
  return me;
}

/*----------------------------------------------------------------------
  BM_bufferFree
  Frees the memory associated with a dynamic buffer
  ----------------------------------------------------------------------*/
void BM_bufferFree (BM_dyn_buffer * me)
{
  if (!me)
    return;

  if (me->buffer)
    TtaFreeMemory (me->buffer);
  TtaFreeMemory (me);
}

/*----------------------------------------------------------------------
  BM_bufferContent
  Returns the content of a dynamic buffer
  ----------------------------------------------------------------------*/
char * BM_bufferContent (BM_dyn_buffer * me)
{
  if (!me)
    return NULL;

  return me->buffer;
}

/*----------------------------------------------------------------------
  BM_bufferClear
  Clears the content  of a dynamic buffer
  ----------------------------------------------------------------------*/
void BM_bufferClear (BM_dyn_buffer * me)
{
  if (!me || !me->buffer)
    return;

  me->buffer[0] = EOS;
}

/*----------------------------------------------------------------------
  BM_bufferCat
  reallocates memory and concatenates a string into buffer	
  ----------------------------------------------------------------------*/
void BM_bufferCat (BM_dyn_buffer *me, char *src)
{
  void               *status;
  int                 lg;

  lg = strlen (src) + 1;
  if ((unsigned int)strlen (me->buffer) + lg > me->lgbuffer)
    {
      /* it is necessary to extend the buffer */
      if (lg < PARAM_INCREMENT)
	lg = PARAM_INCREMENT;
      status = TtaRealloc (me->buffer, sizeof (char) * (me->lgbuffer + lg));      
      if (status != NULL)
	{
	  me->buffer = (char *)status;
	  me->lgbuffer += lg;
	  strcat (me->buffer, src);
	}
    }
  else
    strcat (me->buffer, src);
}

/*----------------------------------------------------------------------
  BM_bufferCopy
  reallocates memory and copies a string into buffer	
  ----------------------------------------------------------------------*/
void BM_bufferCopy (BM_dyn_buffer *me, char *src)
{
  void               *status;
  unsigned int        lg;

  lg = (unsigned int) strlen (src) + 1;
  if (lg > me->lgbuffer)
    {
      /* it is necessary to extend the buffer */
      if (lg < PARAM_INCREMENT)
	lg = PARAM_INCREMENT;
      else
	lg = lg + PARAM_INCREMENT; /* always get some extra chars */
      status = TtaRealloc (me->buffer, sizeof (char) * (lg));
      if (status != NULL)
	{
	  me->buffer = (char *)status;
	  me->lgbuffer = lg;
	  strcpy (me->buffer, src);
	}
    }
  else
    strcpy (me->buffer, src);
}

#ifdef URN
#define URN_TEMPLATE "urn:uid:"

#include <uuid/uuid.h>
#endif /* URN */

/*----------------------------------------------------------------------
  BM_newURN
  Returns a new URN as a static string. The user must not free
  this string.
  ----------------------------------------------------------------------*/
char *BM_newURN (void)
{
#ifdef URN
  uuid_t uu;
  static char char_uu[sizeof(URN_TEMPLATE) + 37];

  strcpy (char_uu, URN_TEMPLATE);

  uuid_generate (uu);
  uuid_unparse (uu, char_uu + sizeof (URN_TEMPLATE) - 1);
  printf ("Your urn for today will be %s\n", char_uu);
  uuid_clear (uu);

  return (char_uu);
#else
  return NULL;
#endif /* URN */
}
