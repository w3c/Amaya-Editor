/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2003
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
      List_delAll (&list, (void *) TtaFreeMemory);
    }
  if (me->self_url)
    TtaFreeMemory (me->self_url);
  if (me->context)
    TtaFreeMemory (me->context);
  if (me->bookmarks)
    TtaFreeMemory (me->bookmarks);
  if (me->title)
    TtaFreeMemory (me->title);
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
  if (src->author)
    me->author = TtaStrdup (src->author);
  if (src->created)
    me->created = TtaStrdup (src->created);
  if (src->modified)
    me->modified = TtaStrdup (src->modified);
  if (src->description)
    me->description = TtaStrdup (src->description);

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
      me = cur->object;
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
  return ((BookmarkP) object)->modified;
}

/*-----------------------------------------------------------
  BM_expandBookmarks
------------------------------------------------------------*/
List *BM_expandBookmarks (List **list)
{
  List *cur;
  List *cur2;
  List *new_list = NULL;
  List *cur_new;
  List *parent_url_list;
  BookmarkP item;
  BookmarkP clone;
  
  cur = *list;
  cur_new = new_list;

  while (cur)
    {
      item = (BookmarkP) cur->object;
      parent_url_list = item->parent_url_list;
      item->parent_url_list = NULL;
      if (List_count (parent_url_list) == 1)
	{
	  item->parent_url = (char *) parent_url_list->object;
	  parent_url_list->object = NULL;
	  List_delAll (&parent_url_list, NULL);
	  List_add (&new_list, (void *) item);
	}
      else
	{
	  cur2 = parent_url_list;
	  while (cur2)
	    {
	      /* not sure if this will work */
	      clone = Bookmark_copy (item);
	      clone->parent_url = (char *) cur2->object;
	      cur2->object = NULL;
	      List_add (&new_list, (void *) clone);
	      cur2 = cur2->next;
	    }
	  Bookmark_free (item);
	  List_delAll (&parent_url_list, NULL);
	}
      cur = cur->next;
    }
  List_delAll (list, NULL);
  return new_list;
}



