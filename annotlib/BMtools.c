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
  BMList_delItem
  ------------------------------------------------------------*/
ThotBool BMList_delItem (void *me)
{
  if (!me)
    return FALSE;

  Bookmark_free ((BookmarkP) me);

  return TRUE;
}

/*-----------------------------------------------------------
  BM_GetMDate
  ------------------------------------------------------------*/
char * BM_GetMDate (void *object)
{
  return ((BookmarkP) object)->modified;
}




