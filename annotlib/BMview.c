/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * BMview.c : module for displaying bookmarks using the S and P schemas.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 */

/*  amaya includes */
#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"

/* annotlib inckudes */
#include "annotlib.h"
#include "f/ANNOTtools_f.h"

/* bookmarks includes */
#include "bookmarks.h"
#include "Topics.h"
#include "f/BMevent_f.h"

#include "f/BMfile_f.h"
#include "f/BMtools_f.h"
#include "f/BMview_f.h"

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
Element BM_topicGetCreateContent (Document doc, Element topic_item)
{
  ElementType elType;
  Element el, el2; 

  elType = TtaGetElementType (topic_item);
  
  elType.ElTypeNum = Topics_EL_Topic_content;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  if (!el) 
    {
       el = TtaNewElement (doc, elType);
       el2 = TtaGetLastChild (topic_item);
       TtaInsertSibling (el, el2, FALSE, doc);
    }

  return el;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void BM_topicSetTitle (Document doc, Element topic_item, char *title)
{
  ElementType elType;
  Element el;

  if (!topic_item || doc == 0)
    return;

 /* topic title  */
  elType = TtaGetElementType (topic_item);
  elType.ElTypeNum = Topics_EL_Topic_title;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  el = TtaGetFirstChild (el);
  if (title)
    TtaSetTextContent (el, (unsigned char *)title, TtaGetDefaultLanguage (), doc); 
  else
    TtaSetTextContent (el, (unsigned char *)"", TtaGetDefaultLanguage (), doc); 
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static char * BM_topicGetTitle (Element topic_item)
{
  ElementType elType;
  Element el;
  char *buffer = NULL;
  int length;
  Language  lang;

  if (!topic_item)
    return NULL;

 /* topic title  */
  elType = TtaGetElementType (topic_item);
  elType.ElTypeNum = Topics_EL_Topic_title;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  el = TtaGetFirstChild (el);
  if (el)
    {
      length = TtaGetTextLength (el) + 1;
      buffer = (char *)TtaGetMemory (length);
      TtaGiveTextContent (el, (unsigned char *)buffer, &length, &lang);
    }
  return buffer;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
char * BM_topicGetModelHref (Element topic_item)
{
  ElementType elType;
  Attribute attr;
  AttributeType attrType;
  char *buffer = NULL;
  int length;
  
  if (!topic_item)
    return NULL;

  elType = TtaGetElementType (topic_item);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
  attr = TtaGetAttribute (topic_item, attrType);
  if (attr)
    {
       length = TtaGetTextAttributeLength (attr) + 1;
       buffer = (char *)TtaGetMemory (length);
       TtaGiveTextAttributeValue (attr, buffer, &length);
    }
  return buffer;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
Element BM_topicAdd (Document doc, Element topic_item)
{
  ElementType      elType;
  Element          topic_content, item, el;
  Attribute        attr;
  AttributeType    attrType;
  DisplayMode      dispMode;

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  if (topic_item)
    {
      /* find the topic content */
      topic_content = BM_topicGetCreateContent (doc, topic_item);
    }
  else
    {
      /* it's the first element of a whole bookmark thread */
      topic_content = TtaGetRootElement (doc);
    }

  elType = TtaGetElementType (topic_content);
      
  /* create the item itself */
  elType.ElTypeNum = Topics_EL_Topic_item;
  item = TtaNewTree (doc, elType, "");
      
  /* attach it to the tree */
  el = TtaGetLastChild (topic_content);
  if (!el)
    TtaInsertFirstChild (&item, topic_content, doc);
  else
    TtaInsertSibling (item, el, FALSE, doc);

  /* give it a default attribute (open) */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Open_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (item, attr, doc);
  TtaSetAttributeValue (attr, Topics_ATTR_Open__VAL_Yes_, item, doc);

 /* make the text part read only */
  elType.ElTypeNum = Topics_EL_Topic_title;
  el = TtaSearchTypedElement (elType, SearchInTree, item);
  TtaSetAccessRight (el, ReadOnly, doc);

  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
      
  return item;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void BM_bookmarkSetTitle (Document doc, Element bookmark_item, char *title)
{
  ElementType elType;
  Element el;

  if (!bookmark_item || doc == 0)
    return;

 /* topic title  */
  elType = TtaGetElementType (bookmark_item);
  elType.ElTypeNum = Topics_EL_Bookmark_title;
  el = TtaSearchTypedElement (elType, SearchInTree, bookmark_item);
  el = TtaGetFirstChild (el);
  if (title)
    TtaSetTextContent (el, (unsigned char *)title, TtaGetDefaultLanguage (), doc); 
  else
    TtaSetTextContent (el, (unsigned char *)"", TtaGetDefaultLanguage (), doc); 
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
Element BM_bookmarkAdd (Document doc, Element topic_item)
{
  ElementType    elType;
  Element        topic_content, item, el;
  DisplayMode    dispMode;

  /* find the topic content */
  topic_content = BM_topicGetCreateContent (doc, topic_item);
  /* if there's nothing, don't do anything */
  if (!topic_content)
    return (Element) NULL;

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);



  elType = TtaGetElementType (topic_content);

  /* create the item itself */
  elType.ElTypeNum = Topics_EL_Bookmark_item;
  item = TtaNewTree (doc, elType, "");

  /* attach it to the tree */
  el = TtaGetLastChild (topic_content);
  if (!el)
    TtaInsertFirstChild (&item, topic_content, doc);
  else
    TtaInsertSibling (item, el, FALSE, doc);

  /* make the text part read only */
  elType.ElTypeNum = Topics_EL_Bookmark_title;
  el = TtaSearchTypedElement (elType, SearchInTree, item);
  TtaSetAccessRight (el, ReadOnly, doc);

 /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  return item;
}


/*-----------------------------------------------------------------------
  BM_InitItem
  -----------------------------------------------------------------------*/
void BM_InitItem (Document doc, Element el, BookmarkP me)
{
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  char               *ptr;

  /* initialize */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (me->isTopic)
    {
      /* topic title  */
      if (me->title && *me->title)
	ptr = me->title;
      else
	ptr = me->self_url;
      BM_topicSetTitle (doc, el, ptr);
    }
  else
    {
      /* bookmark title  */
      /* if there's no title, use the recalls property. */
      if (me->title && *me->title)
	ptr = me->title;
      else if (me->bookmarks)
	ptr = me->bookmarks;
      else
	ptr = me->self_url;
      BM_bookmarkSetTitle (doc, el, ptr);
      /* bookmarks href */
      attrType.AttrTypeNum = Topics_ATTR_HREF_;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, me->bookmarks, el, doc);
    }

  /* model href */
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, doc);
  TtaSetAttributeText (attr, me->self_url, el, doc);
}

/*-----------------------------------------------------------------------
  BM_AddItem
  -----------------------------------------------------------------------*/
Element BM_AddItem (Document doc, BookmarkP me)
{
  ElementType    elType;
  Element        root, item, el, rootOfThread;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

   /* point to the first node */
  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);

  /* point to the home topic */
  rootOfThread = TtaGetFirstChild (root);

  if (!rootOfThread)
    el = NULL; /* it's the first one in the thread */
  else
    {
      /* try to find where to insert the element */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
      
      /* for the moment, we suppose that bookmarks always belong to a
	 topic and that this topic is described in the bookmark file */
      if (me->parent_url)
	{
	  root = rootOfThread;
	  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
	  while (el)
	    {
	      i = TtaGetTextAttributeLength (attr) + 1;
	      url = (char *)TtaGetMemory (i);
	      TtaGiveTextAttributeValue (attr, url, &i);
	      if (!strcasecmp (url, me->parent_url))
		{
		  TtaFreeMemory (url);
		  break;
		}
	      TtaFreeMemory (url);
	      root = el;
	      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
	    }
	}
      else
	el = NULL;

      if (el == NULL)
	/* we didn't find the Topic where to store it, so we insert it as a child of the 
	   home topic */
	el = rootOfThread;
    }

  if (me->isTopic)
    item = BM_topicAdd (doc, el);
  else
    item = BM_bookmarkAdd (doc, el);
  
  /* init the thread item elements */
  if (item)
    BM_InitItem (doc, item,  me);

  return (item);
}

/*-----------------------------------------------------------------------
  BM_InitDocumentStructure
  -----------------------------------------------------------------------*/
void BM_InitDocumentStructure (Document doc, List *bm_list)
{
  List            *cur;
  BookmarkP        bookmark;
  DisplayMode      dispMode;
  Element          el;
  ElementType    elType;

#if 0
  ElementType    elType;
  Element        root;
  Element        el;

  /* point to the first node */
  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);

  /* point to the home topic */
  el = TtaGetFirstChild (root);
  BM_topicSetTitle (doc, el, "My home topics");

  /* A sample test that creates the following structure:
     Bookmark
     Topic
       Topic
         Bookmark
       Bookmark
  */
  BM_bookmarkAdd (doc, el);
  el = BM_topicAdd (doc, el);
  BM_topicAdd (doc, el);
  el2 = BM_topicAdd (doc, el);
  BM_bookmarkAdd (doc, el);
  BM_bookmarkAdd (doc, el2);
#endif

  /* lame! Erase the first topic as I don't know how to say this in
     S */
  el = TtaGetRootElement (doc);
  el = TtaGetFirstChild (el);

  /* @@@ */
  elType = TtaGetElementType (el);

  if (el)
    {
      TtaUnselect (doc);
      TtaDeleteTree (el, doc);
    }

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, NoComputedDisplay);

  cur = bm_list;
  while (cur)
    {
      bookmark = (BookmarkP) cur->object;
      /* @@@ JK: @@@ */

      BM_AddItem (doc, bookmark);
      cur = cur->next;
    }

  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
}

/*-----------------------------------------------------------------------
  BM_CloseTopic
  Takes a list representing the content of a topic and opens it
  -----------------------------------------------------------------------*/
void BM_CloseTopic (Document doc, Element topic_item)
{
  Element        el;
  ElementType    elType;

  if (!topic_item)
    return;

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (doc, NoComputedDisplay);

  elType = TtaGetElementType (topic_item);
  
  elType.ElTypeNum = Topics_EL_Topic_content;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  if (el)
    TtaDeleteTree (el, doc);

  /* show the document */
  TtaSetDisplayMode (doc, DisplayImmediately);
}


/*-----------------------------------------------------------------------
  BM_OpenTopic
  Takes a list representing the content of a topic and opens it
  -----------------------------------------------------------------------*/
void BM_OpenTopic (Document doc, List *bm_list)
{
  List          *cur;
  BookmarkP      bookmark;

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (doc, NoComputedDisplay);

  cur = bm_list;
  while (cur)
    {
      bookmark = (BookmarkP) cur->object;
      BM_AddItem (doc, bookmark);
      cur = cur->next;
    }
  /* show the document */
  TtaSetDisplayMode (doc, DisplayImmediately);
}

/*-----------------------------------------------------------------------
  requested_doc == 0 for the default bookmark view
  -----------------------------------------------------------------------*/
Document BM_NewDocument (Document requested_doc, int ref)
{
  Document doc;
  char *ptr;

  /* I would probably need to do here the same initializations as in LoadDocument
     or even call that function instead. */

  if (requested_doc != 0)
    {
      ptr = DocumentURLs[requested_doc];
      DocumentURLs[requested_doc] = NULL;
      doc = requested_doc;
    }
  else
    {
      ptr = "bookmarks.rdf";
      doc = InitDocAndView (requested_doc, (char *) ((ref == 0) ? "bookmarks.rdf" : ptr), 
			    docBookmark, 0, FALSE, 
			    L_Other, CE_ABSOLUTE);
    }


  if (doc == 0)
    {
      if (requested_doc != 0 && ptr)
	TtaFreeMemory (ptr);
      return 0;
    }
  else if (requested_doc != 0)
    {
      DocumentURLs[requested_doc] = ptr;
    }

  /*
  ** initialize the document 
  */

  if (requested_doc != doc)
    {
      if (DocumentURLs[doc])
	TtaFreeMemory (DocumentURLs[doc]);
      ptr = GetLocalBookmarksFile ();
      DocumentURLs[doc] = TtaStrdup (ptr);
      TtaSetTextZone (doc, 1, DocumentURLs[doc]);
      
      if (DocumentMeta[doc])
	DocumentMetaClear (DocumentMeta[doc]);
      else
	DocumentMeta[doc] = DocumentMetaDataAlloc ();
      
      /* set the charset to be UTF-8 by default */
      TtaSetDocumentCharset (doc, TtaGetCharset ("UTF-8"), FALSE);
    }

  return (doc);
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static Element BM_GetFirstChild (Element topic_item)
{
  Element el;
  ElementType elType;

  elType = TtaGetElementType (topic_item);
  elType.ElTypeNum = Topics_EL_Topic_item;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  return (el);
}

/*----------------------------------------------------------------------
  BM_topicsPrune
  ---------------------------------------------------------------------*/
void BM_topicsPrune (Document doc, BookmarkP me)
{
  Element el, root;
  ElementType elType;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  if (doc == 0 || !me || !me->self_url)
    return;

   /* point to the first node */
  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);

  if (!root)
    return;

  /* try to find the topic and delete its children */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;

  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (!strcasecmp (url, me->self_url))
	{
	  TtaFreeMemory (url);
	  break;
	}
      TtaFreeMemory (url);
      root = el;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }
  if (el)
    {
    /* delete the subtopics, if they exist */
      TtaDeleteTree (el, doc);
      /* 
      elType.ElTypeNum = Topics_EL_Topic_content;
      el = TtaSearchTypedElement (elType, SearchInTree, el);
      if (el)
	TtaDeleteTree (el, doc);
      */
    }
}


/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static void RecursiveInitTreeWidget (ThotWidget tree,
				      ThotWidget parent,
				      ThotWidget sibling,
				      Element root_el)
{
  ThotWidget tree_item, previous;
  char *label;
  Element el, child_el;
  ThotBool select;
  el = root_el;

  previous = sibling;

  while (el) 
    {
      /* insert the element */
      /* get its label */
      label = BM_topicGetTitle (el);
      /* is it selected? */
      select = BM_topicIsSelected (el);

      tree_item = TtaAddTreeItem (tree, parent, previous, label, select, 
				  TRUE, (void *) el);

      if (label)
	TtaFreeMemory (label);

      /* insert all its children recusrively */
      child_el = BM_GetFirstChild (el);
      if (child_el)
	{
	  RecursiveInitTreeWidget (tree, tree_item, NULL, child_el);
	}
      /* do the same with its siblings */
      TtaNextSibling (&el);
      previous = tree_item;
    }
}

/*----------------------------------------------------------------------
  BM_InitTreeWidget
  Initializes the tree widget according to the topic tree
  ----------------------------------------------------------------------*/
void BM_InitTreeWidget (ThotWidget tree, Document TopicTree)
{
  Element el;
  ElementType elType;
  
  el = TtaGetRootElement (TopicTree);
  el = TtaGetFirstChild (el);

  elType = TtaGetElementType (el);

  RecursiveInitTreeWidget (tree, NULL, NULL, el);
}

/*-----------------------------------------------------------------------
  RecursiveDumpTree
  -----------------------------------------------------------------------*/
static void RecursiveDumpTree (Element root_el, List **list)
{
  Element el, child_el;
  char *url;

  el = root_el;
  while (el) 
    {
      if (BM_topicIsSelected (el))
	{
	  /* copy the URL */
	  url =  BM_topicGetModelHref (el);
	  List_add (list, (void *) url);
	}

      /* examine its children and sibling */
      child_el = BM_GetFirstChild (el);
      if (child_el)
	RecursiveDumpTree (child_el, list);
      TtaNextSibling (&el);
    }
}

/*----------------------------------------------------------------------
  BM_dumpTopicTreeSelections
  ----------------------------------------------------------------------*/
void BM_dumpTopicTreeSelections (Document TopicTree, List **list)
{
  Element el;
  
  el = TtaGetRootElement (TopicTree);
  el = TtaGetFirstChild (el);
  *list = NULL;

  RecursiveDumpTree (el, list);
}

/*-----------------------------------------------------------
  BM_GetTopicTree
  Creates a thottree with the topics neatly sorted.
  Caller must free the document.
  ------------------------------------------------------------*/
Document BM_GetTopicTree (int ref)
{
  Document doc;
  List *topics =  NULL;
  int count;
  Element el;
  ElementType elType;

  doc = TtaInitDocument ("Topics", "sorted topics", 0);
  if (doc == 0)
    return 0;
  TtaSetDisplayMode (doc, NoComputedDisplay);

  count = Model_dumpAsList (ref, &topics, True);
  if (count > 0)
    BM_bookmarksSort (&topics);

  el = TtaGetRootElement (doc);
  elType = TtaGetElementType (el);
      
  /* create the item itself */
  elType.ElTypeNum = Topics_EL_Topic_item;
  TtaNewTree (doc, elType, "");
  /* set the charset to be UTF-8 by default */
  TtaSetDocumentCharset (doc, TtaGetCharset ("UTF-8"), FALSE);

  BM_InitDocumentStructure (doc, topics);

  List_delAll (&topics, BMList_delItem);

  return (doc);
}

/*-----------------------------------------------------------------------
  BM_topicSelectToggle
  -----------------------------------------------------------------------*/
void BM_topicSelectToggle (Document doc, char *topic_url, ThotBool select)
{
  DisplayMode      dispMode;

  ElementType    elType;
  Element        root, el;
  Attribute      attr;
  AttributeType  attrType;
  char          *url;
  int            i;

  if (!topic_url || !*topic_url)
    return;

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, NoComputedDisplay);

  /* point to the home topic */
  el = TtaGetRootElement (doc);
  if (!el)
    return;
  root = el;

  /* locate the reference */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
      
  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (!strcasecmp (url, topic_url))
	{
	  TtaFreeMemory (url);
	  attrType.AttrTypeNum = Topics_ATTR_Selected_;
	  if (select)
	    {
	      /* mark this topic as selected if it's not yet the case */
	      attr = TtaGetAttribute (el, attrType);
	      if (!attr)
		{
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (el, attr, doc);
		  TtaSetAttributeValue (attr, Topics_ATTR_Selected__VAL_Yes_, el, doc);
		}
	    }
	  else
	    {
	      /* remove the select attribute if it exists*/
	      attr = TtaGetAttribute (el, attrType);
	      if (attr)
		TtaRemoveAttribute (el, attr, doc);
	    }
	  break;
	}
      TtaFreeMemory (url);
      root = el;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }
  
  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
}

/*-----------------------------------------------------------------------
  BM_topicIsSelected
  -----------------------------------------------------------------------*/
ThotBool BM_topicIsSelected (Element el)
{
  ElementType    elType;
  Attribute      attr;
  AttributeType  attrType;

  if (!el)
    return FALSE;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != Topics_EL_Topic_item)
    return FALSE;
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Selected_;
  attr = TtaGetAttribute (el, attrType);
  
  return (attr) ? TRUE : FALSE;
}


/*-----------------------------------------------------------
  BM_topicsPreSelect
  Selects the topics in a topic tree to which bookmark belongs.
  ------------------------------------------------------------*/
void BM_topicsPreSelect (int ref, Document TopicTree, BookmarkP bookmark)
{
  List *topics =  NULL, *list_item;
  int count;

  if (!bookmark)
    {
     /* select the default topic */
      BM_topicSelectToggle (TopicTree, GetHomeTopicURI(), TRUE);
      return;
    }

  if (bookmark->isTopic && bookmark->parent_url)
      BM_topicSelectToggle (TopicTree, bookmark->parent_url, TRUE);
  else
    {
      count = Model_dumpBookmarkTopics (ref, bookmark, &topics);
      
      if (count > 0)
	{
	  list_item = topics;
	  
	  while (list_item) 
	    {
	      BM_topicSelectToggle (TopicTree, (char *) list_item->object, TRUE);
	      list_item = list_item->next;
	    }
	}
      List_delAll (&topics, (ThotBool (*)(void*)) TtaFreeMemory);
    }
}


typedef struct _BM_PasteBuffer {
  int ref;
  char *bookmark_url;
} BM_PasteBuffer;

static BM_PasteBuffer PasteBuffer;

/*-----------------------------------------------------------
  BM_ClearPasteBuffer
  ------------------------------------------------------------*/
void BM_FreePasteBuffer ()
{
  if (PasteBuffer.bookmark_url)
    {
      TtaFreeMemory (PasteBuffer.bookmark_url);
      PasteBuffer.bookmark_url = NULL;
    }
}

/*-----------------------------------------------------------
  BM_Paste
  Returns TRUE all the time to keep the thotlib from doing
  mischief.
  ------------------------------------------------------------*/
ThotBool BM_Paste (Document doc, Element target)
{
  Element el;
  ElementType elType;
  ThotBool result;

  char *parent_href;
  int dest_ref;

  el = target; /* the place where we want to paste to */

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &dest_ref))
    return TRUE;

  /* empty buffer? */
  if (!PasteBuffer.bookmark_url || PasteBuffer.bookmark_url[0] == EOS)
    return TRUE;

  /* @@@ check to see if the ref still exists. The user may have closed the bm file
     in the meatime */

#if 0
  /* get the concerned node */
  el = TtaGetFirstChild (el);
  while (el && value) 
    {
      TtaNextSibling (&el);
      value--;
    }
  if (!el)
    return TRUE;
#endif

  /* get the concerned node */
  elType = TtaGetElementType (el);
  while (el && elType.ElTypeNum != Topics_EL_Bookmark_item 
	 && elType.ElTypeNum != Topics_EL_Topic_item)
    {
      el = TtaGetParent (el);
      if (el)
	elType = TtaGetElementType (el);
    }

  if (!el)
    return TRUE;

  /* We now find the model's reference for the parent topic. */
  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    {
      /* it's a bookmark item, we get its parent topic. */
      el = TtaGetParent (el);  /* topic content */
      el = TtaGetParent (el);  /* topic item */
      elType = TtaGetElementType (el);
    }

  if (elType.ElTypeNum == Topics_EL_Topic_item)
    {
      /* we can only paste things to 
	 topic items */
      parent_href = BM_topicGetModelHref (el);
    }
  else
      parent_href = NULL;

  if (!parent_href)
    return TRUE;

  if (dest_ref != PasteBuffer.ref)
    {
      /* add the new bookmark to the model */
      result = BM_pasteBookmark (dest_ref, PasteBuffer.ref, PasteBuffer.bookmark_url, parent_href);
    }
  else
    {
      /* add a new topic to the bookmark */
      result = BM_addTopicToBookmark (dest_ref, PasteBuffer.bookmark_url, parent_href);
    }

  if (result)
    {
      BM_refreshBookmarkView (dest_ref); /* refresh the bookmark view */
      if (dest_ref == 0)
	{
	  /* save the modified model */
	  BM_save (dest_ref, GetLocalBookmarksFile ());
	}
      else
	{
	  /* save the modified model in the temporary file */
	  BM_tmpsave (dest_ref);
	  /* mark the document as modified. Let the user do the save */
	  TtaSetDocumentModified (doc);
	}
    }
  TtaFreeMemory (parent_href);

  return FALSE;
}

ThotBool BM_PastePost (NotifyOnValue *event)
{
  int dest_ref;
  Element el; /* target is the source element we want to paste */
  Document  doc;
  
  el = event->element; /* the place where we want to paste to */
  doc = event->document;

  return TRUE;

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &dest_ref))
    return TRUE;

  BM_refreshBookmarkView (dest_ref);   /* refresh the bookmark view */

  return TRUE;
}

/*-----------------------------------------------------------
  BM_PasteNew
  This handler is called when the user puts the selection
  in the middle of a title and tries to paste a bookmark there.
  Returns TRUE all the time to keep the thotlib from doing
  mischief.
  ------------------------------------------------------------*/
ThotBool BM_PasteNew (NotifyElement *event)
{
  Element el; 
  ElementType elType;
  Document  doc;

  char *parent_href;
  int dest_ref;

  el = event->element; /* the place where we want to paste to */
  doc = event->document;

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &dest_ref))
    return TRUE;

  /* empty buffer? */
  if (!PasteBuffer.bookmark_url || PasteBuffer.bookmark_url[0] == EOS)
    return TRUE;

  /* get the concerned node */
  elType = TtaGetElementType (el);
  while  (el && elType.ElTypeNum != Topics_EL_Topic_item
	  && elType.ElTypeNum != Topics_EL_Bookmark_item)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }
  if (!el)
    return TRUE;

  /* We now find the model's reference for the parent topic. */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    {
      /* it's a bookmark item, we get its parent topic. */
      el = TtaGetParent (el);  /* topic content */
      el = TtaGetParent (el);  /* topic item */
      elType = TtaGetElementType (el);
    }

  if (elType.ElTypeNum == Topics_EL_Topic_item)
    {
      /* we can only paste things to 
	 topic items */
      parent_href = BM_topicGetModelHref (el);
    }
  else
      parent_href = NULL;

  if (!parent_href)
    return TRUE;
  
  /* add the new bookmark to the model */
  if (BM_pasteBookmark (dest_ref, PasteBuffer.ref, PasteBuffer.bookmark_url, parent_href))
    {
      BM_refreshBookmarkView (dest_ref);   /* refresh the bookmark view */
      if (dest_ref == 0)
	{
	  /* save the modified model */
	  BM_save (dest_ref, GetLocalBookmarksFile ());
	}
      else
	{
	  /* save the modified model in the temporary file */
	  BM_tmpsave (dest_ref);
	  /* mark the document as modified. Let the user do the save */
	  TtaSetDocumentModified (doc);
	}
    }
  TtaFreeMemory (parent_href);

  return TRUE;
}

/*-----------------------------------------------------------
  BM_Copy
  Returns FALSE all the time because we want the thotlib
  to do its usual work.
  ------------------------------------------------------------*/
ThotBool BM_Copy (NotifyElement *event)
{
  Element   el;
  ElementType elType;
  AttributeType    attrType;
  Attribute        attr;

  Document  doc;
  char *bookmark_url;
  int i;

  el = event->element;
  doc = event->document;

  /*
  ** get the model's bookmark ref
  */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (elType.ElTypeNum == Topics_EL_TEXT_UNIT)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }

  if (elType.ElTypeNum != Topics_EL_Bookmark_item)
    return FALSE;

  /* get the bookmarks' model url */
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
  bookmark_url = (char *) TtaGetMemory (i);
  TtaGiveTextAttributeValue (attr, bookmark_url, &i);

  /* we clear it regardless of what happens below */
  BM_FreePasteBuffer ();

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &i))
    return FALSE;
  PasteBuffer.ref = i;

  PasteBuffer.bookmark_url = bookmark_url;

  return FALSE;
}

/*-----------------------------------------------------------
  BM_IgnoreEvent
  Returns FALSE all the time because we want the thotlib
  to do its usual work.
  ------------------------------------------------------------*/
ThotBool BM_IgnoreEvent (NotifyOnTarget *)
{
  return TRUE;
}
