/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2003.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * BMevent.c : interface module between Amaya and the bookmark
 * code.
 *
 * Author: J. Kahan (W3C/ERCIM)
 *
 */


/* annotlib includes */
#include "annotlib.h"
#include "bookmarks.h"

#include "f/ANNOTtools_f.h"
#include "f/ANNOTevent_f.h"

/* bookmark includes */
#include "f/BMfile_f.h"
#include "f/BMmenu_f.h"
#include "f/BMview_f.h"
#include "f/BMtools_f.h"
#include "Topics.h"

/* Amaya includes */
#include "XPointer.h"
#include "XPointer_f.h"
#include "XPointerparse_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"

static char *LocalBookmarksFile;
static char *LocalBookmarksBaseURI;
static char *HomeTopicURI;
static ThotBool BookmarksEnabled;

static ThotBool GetBookmarksEnabledConfirm (ThotBool confirm)
{
  if (!BookmarksEnabled && confirm)
    {
      InitConfirm (0, 0, "Detected multiple instances of Amaya. Disabling bookmarks");
    }

  return BookmarksEnabled;
}

ThotBool GetBookmarksEnabled (void)
{
  return BookmarksEnabled;
}

char *GetLocalBookmarksFile (void)
{
  return LocalBookmarksFile;
}

char *GetLocalBookmarksBaseURI (void)
{
  return LocalBookmarksBaseURI;
}

char *GetHomeTopicURI (void)
{
  return HomeTopicURI;
}

/*
** public API 
*/

/*-----------------------------------------------------------------------
  BM_Init
  -----------------------------------------------------------------------*/
void BM_Init (void)
{
  char *ptr;
  ThotBool test_result;

  BookmarksEnabled = TRUE;

  /* The user may disable bookmarks from the registry */
  TtaGetEnvBoolean ("DISABLE_BOOKMARKS", &test_result);
  
  if (test_result)
    BookmarksEnabled = FALSE;
  else
    {
      /* as we don't have a lock protection for the moment, we bind
	 ourselves to the cache */
      TtaGetEnvBoolean ("ENABLE_CACHE", &test_result);
      if (!test_result)
	BookmarksEnabled = FALSE;
      else
	{
	  if (!HTCacheMode_enabled ())
	    BookmarksEnabled = FALSE;
	}
    }

  /* initialize it anyway, as this gets raptor initialized too */
  redland_init ();
  
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  /* The TopicURL menu */
  InitTopicURL ();

  /* the local bookmark file name */
  ptr =  TtaGetEnvString ("APP_HOME");
  if (ptr != NULL)
    {
      LocalBookmarksFile = TtaGetMemory (strlen (ptr) + strlen (LOCAL_BOOKMARKS_FILE) + 2);
      sprintf (LocalBookmarksFile, "%s%c%s", ptr, DIR_SEP, LOCAL_BOOKMARKS_FILE);
    }
  LocalBookmarksBaseURI = ANNOT_MakeFileURL ((const char *) LocalBookmarksFile);
  HomeTopicURI = TtaGetMemory (strlen (LocalBookmarksBaseURI) + sizeof (HOME_TOPIC_ANCHOR) + 2);
  sprintf (HomeTopicURI, "%s%s", LocalBookmarksBaseURI, HOME_TOPIC_ANCHOR);

  if (TtaFileExist (LocalBookmarksFile))
    {
      BM_parse (LocalBookmarksFile, LocalBookmarksBaseURI);
      /* get the value of the home topic */
    }
  else
    {
      /* the bookmark file is empty, we create it and add the home topic */
      BookmarkP me;
      char *tmp;

      me = Bookmark_new_init (NULL, TRUE);
      strcpy (me->title, HOME_TOPIC_TITLE);
      tmp = TtaConvertMbsToByte (GetAnnotUser (), ISO_8859_1);
      strcpy (me->author, tmp);
      TtaFreeMemory (tmp);
      strcpy (me->self_url, HomeTopicURI);
      me->created = StrdupDate ();
      me->modified = StrdupDate ();
      me->isTopic = TRUE;
      me->parent_url[0] = EOS;
      /* add and save it right now */
      BM_addTopic (me, FALSE);
      Bookmark_free (me);
    }
}

/*-----------------------------------------------------------------------
  BM_FreeConf
  -----------------------------------------------------------------------*/
void BM_FreeConf ()
{
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  TtaFreeMemory (LocalBookmarksFile);
  TtaFreeMemory (LocalBookmarksBaseURI);
}

/*-----------------------------------------------------------------------
  BM_Quit
  -----------------------------------------------------------------------*/
void BM_Quit (void)
{
  /* we always initialize and close redland, because raptor depends on it */
  redland_free ();

  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  /* save the bookmark file */
  BM_save (LocalBookmarksFile);
  BM_FreeConf ();
}

/*-----------------------------------------------------------------------
  BM_CreateBM
  Opens a dialog for bookmarking the URL viewed in doc.
  -----------------------------------------------------------------------*/
void BM_CreateBM (Document doc, View view)
{
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  BM_BookmarkMenu (doc, view, NULL);
}

/*-----------------------------------------------------------------------
  BM_CreateTopic
  Opens a dialog for bookmarking the URL viewed in doc.
  -----------------------------------------------------------------------*/
void BM_CreateTopic (Document doc, View view)
{
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  BM_TopicMenu (doc, view, NULL);
}

/*-----------------------------------------------------------------------
  BM_ViewBookmarks
  Opens a dialog for bookmarking the URL viewed in doc.
  -----------------------------------------------------------------------*/
void BM_ViewBookmarks (Document doc, View view)
{
  List *list = NULL;
  List *items = NULL;
  int count;
  int item_count;
  Document bookmark_doc;

  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  count = Model_dumpAsList (&list, TRUE);
  item_count = Model_dumpAsList (&items, FALSE);
  if (item_count)
    {
      count += item_count;
      items = BM_expandBookmarks (&items);
      list = List_merge (list, items);
    }

  if (count > 0)
    BM_bookmarksSort (&list);

  /* are we viewing the bookmark view? */
  for (bookmark_doc = 1; bookmark_doc < DocumentTableLength; bookmark_doc++)
    if (DocumentTypes[bookmark_doc] == docBookmark)
      break;

  if (bookmark_doc == DocumentTableLength)
    /* it is a new document */
    bookmark_doc = BM_NewDocument ();
  else if (view == 0) /* view == 0 only when called thru the menu bar */
    TtaRaiseView (bookmark_doc, 1);

  /* get the info for each bookmark using the abookmark structure, e.g., calling
     a bmfile function to fill it up with the fields we want, then adding them
     to the model thru BMview.c */
  BM_InitDocumentStructure (bookmark_doc, list);
 
  List_delAll (&list, BMList_delItem);
}

/*-----------------------------------------------------------
  BM_refreshBookmarkView
  ------------------------------------------------------------*/
void BM_refreshBookmarkView (void)
{
  /* @@ JK: I should add here refresh for multiple bookmark views */

  Document doc;

  /* find the bookmark view document */
  for (doc = 1; doc < DocumentTableLength; doc++)
    if (DocumentTypes[doc] == docBookmark)
      break;

  if (doc == DocumentTableLength)
    return; /* not found */

  /* if found, clear the tree and make a new one */
  BM_ViewBookmarks (1, 1);
}

/*-----------------------------------------------------------------------
  BM_ImportTopics
  Imports a topic hierarchy
  -----------------------------------------------------------------------*/
void BM_ImportTopics (Document doc, View view)
{
  char *url, *normalized_url;

  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  url = GetTopicURL (doc, view);
  if (url && !IsHTTPPath (url))
    {
      if (!IsFilePath (url))
	normalized_url = FixFileURL (url);
      else
	normalized_url = url;

      /* @@ JK: warning, the parsing is not so easy. We have to avoid
	 accepting statements that are already in the bookmakrs. Otherwise,
	 we cannot sort them (infinite loop */
      BM_parse (normalized_url, normalized_url);
      if (url != normalized_url)
	TtaFreeMemory (normalized_url);

      /* save the modified model */
      BM_save (LocalBookmarksFile);

      /* refresh the view if found */
      BM_refreshBookmarkView ();
    }
}


/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static Element GetItemElement (Element input)
{
  ElementType elType;
  Element el = input;

  if (!el)
    return ((Element) NULL);

  el = input;
  elType = TtaGetElementType (el);

  /* point to the correct item */
  if (elType.ElTypeNum == Topics_EL_TEXT_UNIT)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }
  if (elType.ElTypeNum == Topics_EL_TEXT_UNIT
      || elType.ElTypeNum == Topics_EL_Bookmark_title
      || elType.ElTypeNum == Topics_EL_Topic_title
      || elType.ElTypeNum == Topics_EL_Topic_content)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }
  return (el);
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static void FollowBookmark_callback (Document doc, int status, char *urlName, 
				     char *outputfile, AHTHeaders *http_headers,
				     void *ctx)
{
  TtaFreeMemory ((char *) ctx);
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
ThotBool BM_SimpleRClick (NotifyElement *event)
{
  Document doc;
  Element el;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              i;
  char            *url;
  List            *dump;
  DisplayMode      dispMode;

  doc = event->document;
  el = event->element;

  /* point to the correct item */
  el = GetItemElement (el);
  if (!el)
    return FALSE;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    {
      /* get the target URL */
      attrType.AttrTypeNum = Topics_ATTR_HREF_;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
	return FALSE;
      
      i = TtaGetTextAttributeLength (attr);
      if (i < 1)
	{
	  /* bookmark seems empty. We just return */
	  return FALSE;
	}
      
      i++;
      url = TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      
      GetAmayaDoc (url, NULL, 0, doc, 
		   CE_RELATIVE, FALSE, 
		   (void *) FollowBookmark_callback, 
		   (void *) url, UTF_8);
      
      /* don't let Thot perform the normal operation */
      return TRUE;
    }
  else if (elType.ElTypeNum == Topics_EL_Topic_item)
    {
      /* avoid refreshing the document while holophrasting */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);

      attrType.AttrTypeNum = Topics_ATTR_Open_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	{
	  /* TtaHolophrastElement (el, TRUE, doc); */
	  /* change the attribute value */
	  TtaRemoveAttribute (el, attr, doc);
	  attrType.AttrTypeNum = Topics_ATTR_Closed_;
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (el, attr, doc);
	  TtaSetAttributeValue (attr, Topics_ATTR_Closed__VAL_Yes, el, doc);
	  /* remove the contents of this topic */
	  BM_CloseTopic (doc, el);
	}
      else
	{
	  attrType.AttrTypeNum = Topics_ATTR_Closed_;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr)
	    {
	      /* TtaHolophrastElement (el, FALSE, doc); */
	      /* change the attribute value */
	      TtaRemoveAttribute (el, attr, doc);
	      attrType.AttrTypeNum = Topics_ATTR_Open_;
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, attr, doc);
	      TtaSetAttributeValue (attr, Topics_ATTR_Open__VAL_Yes_, el, doc);
	      /* dump the topics related to this one */
	      attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
	      attr = TtaGetAttribute (el, attrType);
	      i = TtaGetTextAttributeLength (attr);
	      if (i > 0)
		{
		  /* allocate some memory: length of name + 6 cars for noname */
		  url = TtaGetMemory (i + 1);
		  TtaGiveTextAttributeValue (attr, url, &i);
		  Model_dumpTopicAsList (&dump, url, TRUE);
		  TtaFreeMemory (url);
		  BM_OpenTopic (doc, dump->next);
		  List_delAll (&dump, BMList_delItem);
		}
	    }
	}

      /* show the document */
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
      if (!attr)
	return FALSE;
      return TRUE;
    }
  else
    return FALSE;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
ThotBool BM_ShowProperties (NotifyElement *event)
{
  Document doc;
  Element el;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              i;
  char            *url;
  BookmarkP        bookmark;

  doc = event->document;
  el = event->element;

 /* point to the correct item */
  el = GetItemElement (el);
  if (!el)
    return FALSE;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (elType.ElTypeNum == Topics_EL_Bookmark_item
      || elType.ElTypeNum == Topics_EL_Topic_item)
    {
      /* get the target URL */
      attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
	return FALSE;
      
      i = TtaGetTextAttributeLength (attr);
      if (i < 1)
	{
	  /* bookmark seems empty. We just return */
	  return FALSE;
	}
      
      i++;
      url = TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (elType.ElTypeNum == Topics_EL_Bookmark_item)
	bookmark = BM_getItem (url, FALSE);
      else
	  bookmark = BM_getItem (url, TRUE);
      TtaFreeMemory (url);

      if (!bookmark)
	  return FALSE;

      bookmark->isUpdate = TRUE;

      if (elType.ElTypeNum == Topics_EL_Bookmark_item)
	BM_BookmarkMenu (doc, 1, bookmark);
      else
	{
	  bookmark->isTopic = TRUE;
	  BM_TopicMenu (doc, 1, bookmark);
	}
      
      /* free the temporary bookmark structure */
      Bookmark_free (bookmark);

      /* don't let Thot perform the normal operation */
      return TRUE;
    }
  else
    return FALSE;
}

/*-----------------------------------------------------------------------
  BM_ItemDelete
  -----------------------------------------------------------------------*/
ThotBool BM_ItemDelete (NotifyElement *event)
{
  Document doc;
  Element          el, el2;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              i;
  char            *url;
  char            *topic_url;
  List            *dump;
  DisplayMode      dispMode;
  ThotBool         isTopic;
  ThotBool         isHomeTopic;

  doc = event->document;
  el = event->element;

  /* point to the correct item */
  el = GetItemElement (el);
  if (!el)
    return TRUE;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    isTopic = FALSE;
  else
    isTopic = TRUE;

  /* get the target URL */
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return TRUE;
      
  i = TtaGetTextAttributeLength (attr);
  if (i < 1)
    {
      /* item seems empty. We just return */
      return TRUE;
    }
      
  i++;
  url = TtaGetMemory (i);
  TtaGiveTextAttributeValue (attr, url, &i);
  
  /* check if the user selected the home topic */
  if (isTopic && !strcmp (url, HomeTopicURI))
    isHomeTopic = TRUE;
  else
    isHomeTopic = FALSE;

  /*
  ** remove the item(s) from the model
  */
  if (isTopic)
    {
      /* get a list of all items in the topic and remove them */
      Model_dumpTopicAsList (&dump, url, FALSE);
      if (isHomeTopic)
	BM_deleteItemList (url, dump->next);
      else
	BM_deleteItemList (url, dump);
      List_delAll (&dump, BMList_delItem);
    }
  else
    {
      /* get the model href of the parent topic for this bookmark */
      el2 = TtaGetParent (el);
      if (el2)
	elType = TtaGetElementType (el2);
      while (el2 && elType.ElTypeNum != Topics_EL_Topic_item)
	{
	  el2 = TtaGetParent (el2);
	  if (el2)
	  elType = TtaGetElementType (el2);
	}
      if (!el2)
	return TRUE; /* didn't find anything! We just return */
      attr = TtaGetAttribute (el2, attrType);
      if (!attr)
	return TRUE;
      i = TtaGetTextAttributeLength (attr);
      if (i < 1)
	{
	  /* item seems empty. We just return */
	  return TRUE;
	}
      i++;
      topic_url = TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, topic_url, &i);
      BM_deleteBookmarkItem (topic_url, url);
      TtaFreeMemory (topic_url);
    }
  TtaFreeMemory (url);

  /* save the modified model */
  BM_save (LocalBookmarksFile);

  /*
  **  move the selection up
  */

  /* choose the next element to highlight */
  /* previous item */
  el2 = el;
  TtaPreviousSibling (&el2);
  if (!el2)
    {
      el2 = el;
      /* next item */
      TtaNextSibling (&el2);
    }

  if (!el2)
    {
      /* there are no siblings, get its parent */
      el2 = TtaGetParent (el);
      el2 =  GetItemElement (el2);
    }

  elType = TtaGetElementType (el2);
  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    {
      /* select the next bookmark sibling */
      elType.ElTypeNum = Topics_EL_Bookmark_title;
      el2 = TtaSearchTypedElement (elType, SearchInTree, el2);
      el2 = TtaGetFirstChild (el2);
      TtaSelectString (doc, el2, 1, 0);
    }
  else /* select the next topic sibling or parent */
    {
      elType.ElTypeNum = Topics_EL_Topic_title;
      el2 = TtaSearchTypedElement (elType, SearchInTree, el2);
      el2 = TtaGetFirstChild (el2);
      TtaSelectString (doc, el2, 1, 0);
    }

  /*
  ** remove the items from the tree 
  */

  /* avoid refreshing the document while doing the tree operation */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  if (isHomeTopic)
    {
      /* delete the container if it exists */
      elType.ElTypeNum = Topics_EL_Topic_content;
      el2 = TtaSearchTypedElement (elType, SearchInTree, el);
      if (el2)
	TtaDeleteTree (el2, doc);
    }
  else
    {
      el2 = TtaGetParent (el);
      TtaDeleteTree (el, doc);

      /* delete the topic container if there is no other sibling */
      elType = TtaGetElementType (el2);
      if (elType.ElTypeNum == Topics_EL_Topic_item)
	{
	  el = TtaGetFirstChild (el2);
	  if (!el)
	    TtaDeleteTree (el2, doc);
	}
    }

  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  /* refresh the bookmark and topic widgets if they are open */
  BM_RefreshTopicTree ();

  /* don't let Thot perform the normal operation */
  return TRUE;
}

