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
  BM_Open
  Opens a given rdf file and, if it contains bookmarks, keeps it in
  memory and returns TRUE.
  Otherwise, returns FALSE.
  @ Keep the mime type?
  -----------------------------------------------------------------------*/
ThotBool BM_Open (char *url, char *tmpfile)
{
  int ref;
  ThotBool result;
  char *normalized_url;

  /* it may be that we need to do a workaround for this protection */
  if (!GetBookmarksEnabledConfirm (FALSE) || !tmpfile 
      || ! TtaFileExist (tmpfile))
    return FALSE;
  
  if (!IsHTTPPath (url) && !IsFilePath (url))
    normalized_url = FixFileURL (url);
  else
    normalized_url = url;

  /* initialize it anyway, as this gets raptor initialized too */
  ref = redland_init (normalized_url, tmpfile, FALSE);

  BM_parse (ref, tmpfile, normalized_url);

  if (normalized_url != url)
    TtaFreeMemory (normalized_url);

  if (BM_containsBookmarks (ref))
    {
      int count;
      int item_count;
      List *list = NULL;
      List *items = NULL;

      /* set the doc that's open */
      result = TRUE;
      count = Model_dumpAsList (ref, &list, TRUE);
      item_count = Model_dumpAsList (ref, &items, FALSE);
      if (item_count)
	{
	  count += item_count;
	  items = BM_expandBookmarks (&items);
	  list = List_merge (list, items);
	}
      /* BM_InitDocumentStructure (doc, list); */
      List_delAll (&list, BMList_delItem);
    }
  else
    {
      redland_free (ref, FALSE);
      result = FALSE;
    }
  return result;
}

/*-----------------------------------------------------------------------
  BM_UpdateTempFile
  Updates the value of the temporary file to which bookmarks can be
  stored.
  -----------------------------------------------------------------------*/
ThotBool BM_TempFileSet (char *url, char *tmpfile)
{
  int ref;
  ThotBool res;

  if (BM_Context_reference (url, &ref))
    res = BMContext_tmpfileSet (ref, tmpfile);
  else
    res = FALSE;

  return res;
}

/*-----------------------------------------------------------------------
  BM_Close
  Removes a given rdf model from memory
  -----------------------------------------------------------------------*/
ThotBool BM_Close (Document doc)
{
  int ref;
  ThotBool result;

  /* ref = 0 is the default bookmark file; it will stay open until we close
     amaya */
  if (!BM_Context_reference (DocumentURLs[doc], &ref) || ref == 0)
    result = FALSE;
  else
    {
      redland_free (ref, FALSE);
      result = TRUE;
    }
  return result;
}

/*-----------------------------------------------------------------------
  BM_FreeDocumentResource
  Frees all the resources related to a given bookmark document
  -----------------------------------------------------------------------*/
void BM_FreeDocumentResource (Document doc)
{
  if (DocumentTypes[doc] != docBookmark)
    return;

  /* we don't much here yet, but here's where we would add other functions */
  BM_Close (doc);
}


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

  /* the local bookmark file name */
  ptr =  TtaGetEnvString ("APP_HOME");
  if (ptr != NULL)
    {
      LocalBookmarksFile = (char *)TtaGetMemory (strlen (ptr) + strlen (LOCAL_BOOKMARKS_FILE) + 2);
      sprintf (LocalBookmarksFile, "%s%c%s", ptr, DIR_SEP, LOCAL_BOOKMARKS_FILE);
    }
  LocalBookmarksBaseURI = ANNOT_MakeFileURL ((char *) LocalBookmarksFile);
  HomeTopicURI = (char *)TtaGetMemory (strlen (LocalBookmarksBaseURI) + sizeof (HOME_TOPIC_ANCHOR) + 2);
  sprintf (HomeTopicURI, "%s%s", LocalBookmarksBaseURI, HOME_TOPIC_ANCHOR);


  /* initialize it anyway, as this gets raptor initialized too */
  redland_init (LocalBookmarksBaseURI, LocalBookmarksFile, TRUE);
  
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  /* The TopicURL menu */
  InitTopicURL ();

  if (TtaFileExist (LocalBookmarksFile))
    {
      BM_parse (0, LocalBookmarksFile, LocalBookmarksBaseURI);
      /* get the value of the home topic */
    }
  else
    {
      /* the bookmark file is empty, we create it and add the home topic */
      BookmarkP me;
      char *tmp;

      me = Bookmark_new_init (NULL, TRUE);
      strcpy (me->title, HOME_TOPIC_TITLE);
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)GetAnnotUser (), ISO_8859_1);
      strcpy (me->author, tmp);
      TtaFreeMemory (tmp);
      strcpy (me->self_url, HomeTopicURI);
      me->created = StrdupDate ();
      me->modified = StrdupDate ();
      me->isTopic = TRUE;
      me->parent_url[0] = EOS;
      /* add and save it right now */
      BM_addTopic (0, 0, me, FALSE);
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
  if (GetBookmarksEnabledConfirm (FALSE))
    {
      /* save the local bookmark file */
      BM_save (0, LocalBookmarksFile);
      BM_FreeConf ();
    }
  /* we always initialize and close redland, because raptor depends on it */
  redland_free (0, TRUE);

  /* free the cut/paste buffer */
  BM_FreePasteBuffer ();
}

/*-----------------------------------------------------------------------
  BM_CreateBM
  Opens a dialog for bookmarking the URL viewed in doc.
  -----------------------------------------------------------------------*/
void BM_CreateBM (Document doc, View view)
{
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  /* force the creation on the default file. We need another widget
     to select where to create bookmarks */
  BM_BookmarkMenu (doc, view, 0, NULL);
}

/*-----------------------------------------------------------------------
  BM_CreateTopic
  Opens a dialog for bookmarking the URL viewed in doc.
  -----------------------------------------------------------------------*/
void BM_CreateTopic (Document doc, View view)
{
  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  /* force the creation on the default file. We need another widget
     to select where to create bookmarks */
  BM_TopicMenu (doc, view, 0, NULL);
}

/*-----------------------------------------------------------------------
  BM_ViewBookmarks
  Opens a dialog for viewing the current bookmarks
  -----------------------------------------------------------------------*/
void BM_ViewBookmarks (Document doc, View view, ThotBool isRefresh)
{
  List *list = NULL;
  List *items = NULL;
  int count;
  int item_count;
  int ref;
  Document bookmark_doc;

  if (!GetBookmarksEnabledConfirm (FALSE))
    return;

  if (doc != 0)
    {
      if (!BM_Context_reference (DocumentURLs[doc], &ref))
	return;
    }
  else
    ref = 0;

  count = Model_dumpAsList (ref, &list, TRUE);
  item_count = Model_dumpAsList (ref, &items, FALSE);
  if (item_count)
    {
      count += item_count;
      items = BM_expandBookmarks (&items);
      list = List_merge (list, items);
    }

  if (count > 0)
    BM_bookmarksSort (&list);

  if (!isRefresh)
    {
      /* doc == 0 only when called thru the menu bar */
      if (doc == 0)
	{
	  /* are we viewing the bookmark view? */
	  for (bookmark_doc = 1; bookmark_doc < DocumentTableLength; bookmark_doc++)
	    if (DocumentTypes[bookmark_doc] == docBookmark
		&& !strcmp (DocumentURLs[bookmark_doc], LocalBookmarksFile))
	      break;
	  
	  if (bookmark_doc == DocumentTableLength)
	    /* it is a new document */
	    bookmark_doc = BM_NewDocument (0, 0);
	  else if (view == 0) /* view == 0 only when called thru the menu bar */
	    TtaRaiseView (bookmark_doc, 1);
	}
      else
	bookmark_doc = BM_NewDocument (doc, ref);
    }
  else
    bookmark_doc = doc;

  /* get the info for each bookmark using the abookmark structure, e.g., calling
     a bmfile function to fill it up with the fields we want, then adding them
     to the model thru BMview.c */
  BM_InitDocumentStructure (bookmark_doc, list);
  List_delAll (&list, BMList_delItem);
}

/*-----------------------------------------------------------
  BM_refreshBookmarkView
  ------------------------------------------------------------*/
void BM_refreshBookmarkView (int ref)
{
  ThotBool modified;
  int doc;
  Element el;
  int ch, i;
  ElementType elType;
  char *url = NULL;

  doc = BM_getDocumentFromRef (ref);
  if (doc == 0)
    return; /* nothing found */

  /* we save the state because we're clearing the tree and this
     resests all the flags *.bookmark window and
     this is lost */
  modified =  TtaIsDocumentModified (doc);
  /* store the URL of the selected entry */

  if (TtaIsDocumentSelected (doc))
    {
      TtaGiveFirstSelectedElement (doc, &el, &ch, &i);
      elType = TtaGetElementType (el);
      while (el && elType.ElTypeNum != Topics_EL_Bookmark_item
	     && elType.ElTypeNum != Topics_EL_Topic_item)
	{
	  el = TtaGetParent (el);
	  elType = TtaGetElementType (el);
	}
      if (el)
	url = BM_topicGetModelHref (el);
      /* get the model's URL */
    }

  /* @@ JK: how to give the correct doc? */
  /* if found, clear the tree and make a new one */
  BM_ViewBookmarks (doc, 1, TRUE);
  if (url)
    {
      /* search the URL of the previous selected entry. If it is there,
	 reselect it */
      TtaFreeMemory (url);
    }
  if (modified)
    TtaSetDocumentModified (doc);
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
      BM_parse (0, normalized_url, normalized_url);
      if (url != normalized_url)
	TtaFreeMemory (normalized_url);

      /* save the modified model */
      BM_save (0, LocalBookmarksFile);

      /* refresh the view if found */
      /* @@ JK: how to choose the topic? Using 0 means we only import topics in the
       default bookmark view */
      BM_refreshBookmarkView (0);
    }
}

/*-----------------------------------------------------------------------
  BM_PasteHandler
  Frontend to the paste function.
  -----------------------------------------------------------------------*/
void BM_PasteHandler (Document doc, View view)
{
  Element el;
  int first, last;

  if (TtaGetSelectedDocument () != doc
      || !TtaIsSelectionEmpty ())
    return; /* selection must be in the same document, and only
	     a caret */

  /* do we need to be careful about what is first and last? */
  TtaGiveFirstSelectedElement (doc, &el, &first, &last);

  if (!el)
    return;

  BM_Paste (doc, el);
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
				     char *outputfile, const AHTHeaders *http_headers,
				     void *ctx)
{
  TtaFreeMemory ((char *) ctx);
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
ThotBool BM_FollowBookmark (NotifyElement *event)
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
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      /*** DO WE HAVE TO CONVERT THIS UTF-8 STRING??? */
      GetAmayaDoc (url, NULL, 0, doc, 
		   CE_RELATIVE, FALSE, 
		   FollowBookmark_callback, 
		   (void *) url);
      
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
		  int ref;

		  if (BM_Context_reference (DocumentURLs[doc], &ref)) 
		    {
		      /* allocate some memory: length of name + 6 cars for noname */
		      url = (char *) TtaGetMemory (i + 1);
		      TtaGiveTextAttributeValue (attr, url, &i);
		      Model_dumpTopicAsList (ref, &dump, url, TRUE);
		      TtaFreeMemory (url);
		      BM_OpenTopic (doc, dump->next);
		      List_delAll (&dump, BMList_delItem);
		    }
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
  int              ref;
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
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);

      /* get the bookmark file reference number */
      if (BM_Context_reference (DocumentURLs[doc], &ref))
	{
	  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
	    bookmark = BM_getItem (ref, url, FALSE);
	  else
	    bookmark = BM_getItem (ref, url, TRUE);
	}
      else
	bookmark = NULL;
      TtaFreeMemory (url);

      if (!bookmark)
	  return FALSE;

      bookmark->isUpdate = TRUE;

      if (elType.ElTypeNum == Topics_EL_Bookmark_item)
	BM_BookmarkMenu (doc, 1, ref, bookmark);
      else
	{
	  bookmark->isTopic = TRUE;
	  BM_TopicMenu (doc, 1, ref, bookmark);
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
  int              ref;

  doc = event->document;
  el = event->element;

  /* point to the correct item */
  el = GetItemElement (el);
  if (!el)
    return TRUE;

  /* get the models reference */
  /* @@ JK: don't know how to save an edited file */
  if (!BM_Context_reference (DocumentURLs[doc], &ref))
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
  url = (char *)TtaGetMemory (i);
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
      Model_dumpTopicAsList (ref, &dump, url, FALSE);
      if (isHomeTopic)
	BM_deleteItemList (ref, url, dump->next);
      else
	BM_deleteItemList (ref, url, dump);
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
	{
	  TtaFreeMemory (url);
	  return TRUE; /* didn't find anything! We just return */
	}
      attr = TtaGetAttribute (el2, attrType);
      if (!attr)
	{
	  TtaFreeMemory (url);
	  return TRUE;
	}
      i = TtaGetTextAttributeLength (attr);
      if (i < 1)
	{
	  /* item seems empty. We just return */
	  TtaFreeMemory (url);
	  return TRUE;
	}
      i++;
      topic_url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, topic_url, &i);
      BM_deleteBookmarkItem (ref, topic_url, url);
      TtaFreeMemory (topic_url);
    }
  TtaFreeMemory (url);

  if (ref == 0)
    {
      /* save the modified model */
      BM_save (ref, LocalBookmarksFile);
    }
  else
    {
      /* save the modified model in the temporary file */
      BM_tmpsave (ref);
      /* mark the document as modified. Let the user do the save */
      TtaSetDocumentModified (doc);
    }

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
  BM_RefreshTopicTree (doc);

  /* don't let Thot perform the normal operation */
  return TRUE;
}
/*----------------------------------------------------------------------
  BM_DocSave
  Writes the bookmark model to filename.
  ----------------------------------------------------------------------*/
ThotBool BM_DocSave (Document doc, char *filename)
{
  int ref;
  ThotBool result;

 if (!BM_Context_reference (DocumentURLs[doc], &ref))
   result = FALSE;
 else 
   result = BM_save (ref, filename);
 
 return result;
}
