/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2008
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
#include "f/BMmenu_f.h"

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
  BM_topicGetModelHref
  returns the URL of the model corresponding to the parent topic
  
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
void BM_seeAlsoSetTitle (Document doc, Element seealso_item, char *title)
{
  ElementType elType;
  Element el;

  if (!seealso_item || doc == 0)
    return;

 /* topic title  */
  elType = TtaGetElementType (seealso_item);
  elType.ElTypeNum = Topics_EL_SeeAlso_title;
  el = TtaSearchTypedElement (elType, SearchInTree, seealso_item);
  el = TtaGetFirstChild (el);
  if (title)
    TtaSetTextContent (el, (unsigned char *)title, TtaGetDefaultLanguage (), doc); 
  else
    TtaSetTextContent (el, (unsigned char *)"", TtaGetDefaultLanguage (), doc); 
}

/*-----------------------------------------------------------------------
  BM_seeAlsoAdd
  -----------------------------------------------------------------------*/
Element BM_seeAlsoAdd (Document doc, Element parent_item)
{
  Element      item, el;
  ElementType  elType;
  ThotBool     isTopic;

  if (!parent_item)
    return NULL;

  elType = TtaGetElementType (parent_item);
  if (elType.ElTypeNum == Topics_EL_Topic_item)
    isTopic = TRUE;
  else if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    isTopic = FALSE;
  else
    return NULL;

  /* is there already a see also container? */
  elType.ElTypeNum = Topics_EL_SeeAlso_content;
  el = TtaSearchTypedElement (elType, SearchInTree, parent_item);
  if (!el)
    {
      item = TtaNewElement (doc, elType);
      /* insert it after the title of the parent_item */
      if (isTopic) 
	elType.ElTypeNum = Topics_EL_Topic_title;
      else
	elType.ElTypeNum = Topics_EL_Bookmark_title;
      el = TtaSearchTypedElement (elType, SearchInTree, parent_item);
      TtaInsertSibling (item, el, FALSE, doc);
      el = item;
    }

  /* now create the see also item */
  elType.ElTypeNum = Topics_EL_SeeAlso_item;
  item = TtaNewTree (doc, elType, "");

  /* insert it (probably it should be the LAST sibling, not the first one) */
  TtaInsertFirstChild (&item, el, doc);

  /* make the text part read only */
  elType.ElTypeNum = Topics_EL_SeeAlso_title;
  el = TtaSearchTypedElement (elType, SearchInTree, item);
  TtaSetAccessRight (el, ReadOnly, doc);

  return item;
}

/*-----------------------------------------------------------------------
  BM_separatorAdd
  -----------------------------------------------------------------------*/
Element BM_separatorAdd (Document doc, Element topic_item)
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
  elType.ElTypeNum = Topics_EL_Separator_item;
  item = TtaNewTree (doc, elType, "");

  /* attach it to the tree */
  el = TtaGetLastChild (topic_content);
  if (!el)
    TtaInsertFirstChild (&item, topic_content, doc);
  else
    TtaInsertSibling (item, el, FALSE, doc);

 /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  return item;
}

/*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
Element BM_topicAdd (Document doc, Element topic_item, ThotBool collapsed)
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
  if (collapsed)
    {
      attrType.AttrTypeNum = Topics_ATTR_Closed_;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (item, attr, doc);
      TtaSetAttributeValue (attr,  Topics_ATTR_Closed__VAL_Yes, item, doc);
    }
  else
    {
      attrType.AttrTypeNum = Topics_ATTR_Open_;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (item, attr, doc);
      TtaSetAttributeValue (attr, Topics_ATTR_Open__VAL_Yes_, item, doc);
    }

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

  if (me->bm_type == BME_TOPIC)
    {
      /* topic title  */
      if (me->title && *me->title)
	ptr = me->title;
      else
	ptr = me->self_url;
      BM_topicSetTitle (doc, el, ptr);
    }
  else if (me->bm_type == BME_BOOKMARK)
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
  else if (me->bm_type == BME_SEEALSO)
    {
      /* seeAlso title  */
      if (me->nickname && *me->nickname)
	ptr = me->nickname;
      else if (me->title && *me->title)
	ptr = me->title;
      else
	ptr = me->bookmarks;
      BM_seeAlsoSetTitle (doc, el, ptr);
      /* seeAlso href */
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
  Element        root, el, rootOfThread;
  Element        item = NULL;
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

  if (me->bm_type == BME_SEEALSO)
    {
      /* find the topic content */
      el = BM_topicGetCreateContent (doc, el);
      if (el)
	{
	  /* find the element to which we must attach the seeAlso */
	  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
	  el = TtaGetFirstChild (el);
	  while (el)
	    {
	      attr = TtaGetAttribute (el, attrType);
	      if (attr)
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
		}
	      TtaNextSibling (&el);
	    }
	}
    }
  
  if (me->bm_type == BME_TOPIC)
    item = BM_topicAdd (doc, el, me->collapsed);
  else if (me->bm_type == BME_BOOKMARK)
    item = BM_bookmarkAdd (doc, el);
  else if (me->bm_type == BME_SEEALSO)
    item = BM_seeAlsoAdd (doc, el);
  else if (me->bm_type == BME_SEPARATOR)
    item = BM_separatorAdd (doc, el);
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
  BM_seeAlsoAdd (doc, el2);
  BM_separatorAdd (doc, el2);  <-- to be added
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

  /* delete the topic entries */
  elType.ElTypeNum = Topics_EL_Topic_content;
  el = TtaSearchTypedElement (elType, SearchInTree, topic_item);
  if (el)
    TtaDeleteTree (el, doc);

  /* delete the seeAlso entries */
  elType.ElTypeNum = Topics_EL_SeeAlso_content;
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
      doc = InitDocAndView (requested_doc,
                            FALSE /* replaceOldDoc */,
                            TRUE /* inNewWindow */,
	                    (char *) ((ref == 0) ? "bookmarks.rdf" : ptr), 
			    docBookmark, 0, FALSE, 
			    L_Bookmarks, 0, CE_ABSOLUTE);
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
      /* update menu entries */
      UpdateEditorMenus (doc);
      
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

  doc = TtaInitDocument ("Topics", "sorted topics", "Topics", 0);
  if (doc == 0)
    return 0;
  TtaSetDisplayMode (doc, NoComputedDisplay);

  count = Model_dumpAsList (ref, &topics, BME_TOPIC);
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

  if (!bookmark || bookmark->self_url[0] == EOS)
    {
     /* select the default topic */
      BM_topicSelectToggle (TopicTree, GetHomeTopicURI(), TRUE);
      return;
    }

  /* this is to help ignore a stray gtk callback */
  if (bookmark->self_url[0] == EOS)
    return;

  if (bookmark->bm_type == BME_TOPIC && bookmark->parent_url)
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
  char *topic_url;
  BookmarkElements item_type;
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
  if (PasteBuffer.topic_url)
    {
      TtaFreeMemory (PasteBuffer.topic_url);
      PasteBuffer.topic_url = NULL;
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
  ThotBool result = FALSE;

  int              dest_ref;
  char            *selected_item_url;
  char            *topic_url;
  ThotBool         isHomeTopic;
  ThotBool         isBlankNode;
  ThotBool         addFirst;
  BookmarkElements item_type;
  char            *bookmark_url = NULL;

  el = target; /* the place where we want to paste to */

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &dest_ref))
    return TRUE;

  /* empty buffer? */
  if (!PasteBuffer.bookmark_url || PasteBuffer.bookmark_url[0] == EOS)
    return TRUE;

  /* paste is only allowed for bookmarks */
  if (PasteBuffer.item_type != BME_BOOKMARK)
    return TRUE;

  /* @@@ check to see if the ref still exists. The user may have closed the bm file
     in the meatime */

  /* get the model's reference for the point of insertion */
  result = BM_GetModelReferences (dest_ref, doc, el,
				 &topic_url, &isHomeTopic,
				 &selected_item_url, &item_type, &isBlankNode);

  if (!result)
    return TRUE;

  if (item_type == BME_TOPIC)
    {
      TtaFreeMemory (topic_url);
      topic_url = selected_item_url;
      selected_item_url =  NULL;
      addFirst = TRUE;
    }
  else
    addFirst = FALSE;

  if (dest_ref != PasteBuffer.ref)
    {
      /* add the new bookmark to the model */
      result = BM_pasteBookmark (dest_ref, PasteBuffer.ref, PasteBuffer.bookmark_url, 
				 topic_url,  &bookmark_url);
      if (result)
	{
	  TtaFreeMemory (PasteBuffer.bookmark_url);
	  PasteBuffer.bookmark_url = bookmark_url;
	  TtaFreeMemory (PasteBuffer.topic_url);
	  PasteBuffer.topic_url = TtaStrdup (topic_url);
	  PasteBuffer.ref = dest_ref;
	}
    }
  else
    {
      /* only do the operation if we're not copying and pasting to the same
	 position */
      if (!selected_item_url
	  || strcmp (PasteBuffer.bookmark_url, selected_item_url))
	{
	  /* add a new topic to the bookmark */
	  
	  result = BM_addTopicToBookmark (dest_ref, PasteBuffer.bookmark_url, topic_url);
	  if (!result)
	    {
	      bookmark_url = PasteBuffer.bookmark_url;
	      /* the bookmark already existed in this topic. It's just a change
		 of order */
	      /* we may need to remember its model input so that we only change
		 the order in the collection */
	      BM_deleteItemCollection (dest_ref, topic_url, PasteBuffer.bookmark_url,
				       &isBlankNode, FALSE);
	      result = TRUE;
	    }
	}
    }
  
  if (result)
    {
      BookmarkP me;

      /* add the bookmark to its new position */
      me = Bookmark_new ();
      me->self_url = TtaStrdup (PasteBuffer.bookmark_url);
      me->bm_type = PasteBuffer.item_type;
      BM_addItemToCollection (dest_ref, topic_url, selected_item_url, me, 
			      addFirst);
      Bookmark_free (me);

      /*
      ** save the modifications
      */
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
      /* refresh the bookmark view */
      BM_refreshBookmarkView (dest_ref);
    }

  if (selected_item_url)
    TtaFreeMemory (selected_item_url);
  TtaFreeMemory (topic_url);

  return FALSE;
}

/*-----------------------------------------------------------
  BM_Move
  Moves an item in a given bookmark file. 
  A move can be done between different documents.
  Returns TRUE all the time to keep the thotlib from doing
  mischief.
  ------------------------------------------------------------*/
ThotBool BM_Move (Document doc, Element target)
{
  Element el;
  ThotBool result = FALSE;

  int              dest_ref;
  char            *selected_item_url;
  char            *topic_url;
  ThotBool         isHomeTopic;
  ThotBool         isBlankNode;
  ThotBool         addFirst;
  BookmarkElements item_type;
  
  el = target; /* the place where we want to paste to */

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &dest_ref))
    return TRUE;

  /* empty buffer? */
  if (!PasteBuffer.bookmark_url || PasteBuffer.bookmark_url[0] == EOS
      || !PasteBuffer.topic_url || PasteBuffer.topic_url[0] == EOS)
    return TRUE;
     
  /* @@@ check to see if the ref still exists. The user may have closed the bm file
     in the meatime */

  /* get the model's reference for the point of insertion */
  result = BM_GetModelReferences (dest_ref, doc, el,
				  &topic_url, &isHomeTopic,
				  &selected_item_url, &item_type, &isBlankNode);

  if (!result)
    return TRUE;
  
  if (item_type == BME_TOPIC)
    {
      TtaFreeMemory (topic_url);
      topic_url = selected_item_url;
      selected_item_url =  NULL;
      addFirst = TRUE;
    }
  else
    addFirst = FALSE;

  if (dest_ref != PasteBuffer.ref)
    {
      /* add the new bookmark to the model */
      result = FALSE;
    }
  else
    {
      BookmarkP me;

      /* only do the operation if we're not copying and pasting to the same
	 position */
      if (!selected_item_url
	  || strcmp (PasteBuffer.bookmark_url, selected_item_url))
	{
	  me = Bookmark_new ();
	  me->self_url = PasteBuffer.bookmark_url;
	  me->bm_type = PasteBuffer.item_type;

	  if (me->bm_type == BME_BOOKMARK)
	    {
	      /* add a new topic to the bookmark */
	      if (strcmp (topic_url, PasteBuffer.topic_url))
		{
		  result = BM_addTopicToBookmark (dest_ref, PasteBuffer.bookmark_url, topic_url);
		  if (result)
		    {
		      /* remove the previous hasTopic property */
		      BM_deleteBookmarkTopic (dest_ref, PasteBuffer.topic_url, 
					      PasteBuffer.bookmark_url, isBlankNode); 
		    }
		}
	      else
		result = TRUE; /* it's the same topic, we're just moving it around */
	    }
	  else if (me->bm_type == BME_TOPIC)
	    {
	      /* try to change the subTopicOf property to make it point to its new parent
		 (the new location can't be a child of the current topic) */
	      result = BM_replaceTopicParent (dest_ref, me->self_url, topic_url);
	    }
	  else if (me->bm_type == BME_SEPARATOR)
	    result = TRUE; /* nothing special to do separators */

	  if (result)
	    {
	      /* the bookmark already existed in this topic. It's just a change
		 of order */
	      /* we may need to remember its model input so that we only change
		 the order in the collection */
	      BM_deleteItemCollection (dest_ref, PasteBuffer.topic_url, me->self_url,
				       &isBlankNode, FALSE);
	      BM_addItemToCollection (dest_ref, topic_url, selected_item_url, me, 
				      addFirst);
	      /* we update the buffer to the current topic position so that
		 the user can move it elsewhere and save him the trouble
		 of having to copy it again */
	      TtaFreeMemory (PasteBuffer.topic_url);
	      PasteBuffer.topic_url = TtaStrdup (topic_url);
	      result = TRUE;
	    }
	}
    }

  if (result)
    {
      BM_refreshBookmarkView (dest_ref); /* refresh the bookmark view */
      /* refresh the bookmark and topic widgets if they are open */
      if (item_type == BME_TOPIC)
	BM_RefreshTopicTree (dest_ref);
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
  
  if (selected_item_url)
    TtaFreeMemory (selected_item_url);
  TtaFreeMemory (topic_url);
  
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

  /* we clear the paste buffer regardless of what happens below */
  BM_FreePasteBuffer ();

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

  /* @@ JK: This is what I was using before for allowing copy only on bookmarks */
#if 0
  if (elType.ElTypeNum != Topics_EL_Bookmark_item)
    return FALSE;
#else
  /* @@ JK: the new code for copying all */
  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    PasteBuffer.item_type = BME_BOOKMARK;
  else if (elType.ElTypeNum == Topics_EL_Separator_item)
    PasteBuffer.item_type = BME_TOPIC;
  else if (elType.ElTypeNum == Topics_EL_Topic_item)
    PasteBuffer.item_type = BME_TOPIC;
  else
    return FALSE;
#endif

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

  /*
  ** get the reference
  */
  if (!BM_Context_reference (DocumentURLs[doc], &i))
    return FALSE;
  PasteBuffer.ref = i;

  /* store the URL in the buffer */
  PasteBuffer.bookmark_url = bookmark_url;

  /*
   * try to find a parent and store its URL 
   */
  el = TtaGetParent (el);
  if (el)
    el = TtaGetParent (el);
  PasteBuffer.topic_url = BM_topicGetModelHref (el);

  return FALSE;
}

/*-----------------------------------------------------------
  BM_IgnoreEvent
  Returns FALSE all the time because we want the thotlib
  to do its usual work.
  ------------------------------------------------------------*/
ThotBool BM_IgnoreEvent (NotifyOnTarget *target)
{
  return TRUE;
}

/*-----------------------------------------------------------
  BM_selectNextItem
  select the next sibling of the element containing item_url
  ------------------------------------------------------------*/
void BM_selectNextItem (Document doc, char *item_url)
{
  Element        el, root;
  ElementType    elType;
  Attribute      attr;
  AttributeType  attrType;
  int            i;
  char          *url;

  if (DocumentTypes[doc] != docBookmark || !item_url || *item_url == EOS)
    return;

  TtaUnselect (doc);

  root = TtaGetRootElement (doc);
  elType = TtaGetElementType (root);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;      

  TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
  while (el)
    {
      i = TtaGetTextAttributeLength (attr) + 1;
      url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, url, &i);
      if (!strcasecmp (url, item_url))
	{
	  TtaFreeMemory (url);
	  break;
	}
      TtaFreeMemory (url);
      root = el;
      TtaSearchAttribute (attrType, SearchForward, root, &el, &attr);
    }

  root = el;

  if (root)
    {
      TtaNextSibling (&el);
      if (!el)
	el = root;
      TtaSelectElement (doc, el);
    }
}

/*-----------------------------------------------------------
  BM_GetModelReferences
  ------------------------------------------------------------*/
ThotBool BM_GetModelReferences (int ref, Document doc, Element sel, 
				char **topic_url, ThotBool *isHomeTopic,
				char **item_url, BookmarkElements *item_type, 
				ThotBool *isBlankIdentifier)
{
  Element          el;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              i;
  char            *selected_item_url;

  el = sel;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (elType.ElTypeNum == Topics_EL_SeeAlso_content
      || elType.ElTypeNum == Topics_EL_C_Empty
      || elType.ElTypeNum == Topics_EL_TEXT_UNIT)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }

  if (elType.ElTypeNum == Topics_EL_Bookmark_title
      || elType.ElTypeNum == Topics_EL_Topic_title)
    {
      el = TtaGetParent (el);
      elType = TtaGetElementType (el);
    }

  if (elType.ElTypeNum == Topics_EL_Bookmark_item)
    {
      *item_type = BME_BOOKMARK;
      *isBlankIdentifier = FALSE;
    }
  else if ( elType.ElTypeNum == Topics_EL_Separator_item)
    {
      *item_type = BME_SEPARATOR;
      *isBlankIdentifier = TRUE;
    }
  else if (elType.ElTypeNum == Topics_EL_Topic_item)
    {
      *item_type = BME_TOPIC;
      *isBlankIdentifier = FALSE;
    }
  else
    return FALSE;

  /* get the target URL */
  attrType.AttrTypeNum = Topics_ATTR_Model_HREF_;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return FALSE;
      
  i = TtaGetTextAttributeLength (attr);
  if (i < 1)
    {
      /* item seems empty. We just return */
      return FALSE;
    }
      
  i++;
  selected_item_url = (char *) TtaGetMemory (i);
  TtaGiveTextAttributeValue (attr, selected_item_url, &i);
  *item_url = selected_item_url;

  /* get the model's reference for the parent topic */
  el = TtaGetParent (el); /* topic content */
  el = TtaGetParent (el); /* topic item */
  *topic_url = BM_topicGetModelHref (el);

  /* check if the user selected the home topic */
  if (*item_type == BME_TOPIC && !strcmp (selected_item_url, GetHomeTopicURI ()))
    *isHomeTopic = TRUE;
  else
    *isHomeTopic = FALSE;

  return TRUE;
}
