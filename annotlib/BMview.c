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

/* bookmarks includes */
#include "bookmarks.h"
#include "Topics.h"

/* annotlib inckudes */
#include "annotlib.h"

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
    TtaSetTextContent (el, title, TtaGetDefaultLanguage (), doc); 
  else
    TtaSetTextContent (el, "", TtaGetDefaultLanguage (), doc); 
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
  TtaSetAttributeValue (attr, Topics_ATTR_Open__VAL_Yes_, el, doc);

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
    TtaSetTextContent (el, title, TtaGetDefaultLanguage (), doc); 
  else
    TtaSetTextContent (el, "", TtaGetDefaultLanguage (), doc); 
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

  /* initialize */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;

  if (me->isTopic)
    {
      /* topic title  */
      BM_topicSetTitle (doc, el, me->title);
      /* model href */
    }
  else
    {
      /* bookmark title  */
      BM_bookmarkSetTitle (doc, el, me->title);
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
	      url = TtaGetMemory (i);
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

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, NoComputedDisplay);

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
  if (el)
    TtaDeleteTree (el, doc);

  cur = bm_list;
  while (cur)
    {
      bookmark = (BookmarkP) cur->object;
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
  -----------------------------------------------------------------------*/
Document BM_NewDocument (void)
{
  Document doc;
  doc = InitDocAndView (0, "bookmarks.rdf", docBookmark, 0, FALSE, L_Other, CE_ABSOLUTE);
  
  if (doc == 0)
    return 0;

  /*
  ** initialize the document 
  */

  /* set the charset to be UTF-8 by default */
  TtaSetDocumentCharset (doc, TtaGetCharset ("UTF-8"), FALSE);

  return (doc);
}



