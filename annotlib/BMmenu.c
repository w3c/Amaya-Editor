/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2003.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * BMmenu.c : configuraiton menuinterface module between Amaya and the bookmark
 * code.
 *
 * Author: J. Kahan (W3C/ERCIM)
 *
 */

/* amaya includes */
#define THOT_EXPORT extern
#include "amaya.h"

#include "annotlib.h"
#include "f/ANNOTtools_f.h"
#include "f/ANNOTevent_f.h"

/* bookmark includes */
#include "bookmarks.h"
#include "BMmenu.h"
#include "f/BMfile_f.h"
#include "f/BMevent_f.h"
#include "f/BMtools_f.h"
#include "f/BMview_f.h"
#include "f/BMmenu_f.h"

/* amaya includes */
#include "AHTURLTools_f.h"

static char    s[MAX_LENGTH]; /* general purpose buffer */

static int       BookmarkBase;
static int       TopicBase;
static int       TopicURLBase;

static BookmarkP aBookmark;
static dynBookmarkP aDynBookmark;
static BookmarkP aTopic;
static dynBookmarkP aDynTopic;
static Document  BTopicTree; /* points to a thot sorted topic tree */
static Document  TTopicTree; /* points to a thot sorted topic tree */

static char TopicDirectory[MAX_LENGTH];
static char TopicFilter[NAME_LENGTH];
static char LastURLTopic[MAX_LENGTH];

/*----------------------------------------------------------------------
  Bookmark_new_init
  ----------------------------------------------------------------------*/
BookmarkP Bookmark_new_init (BookmarkP initial, ThotBool isTopic)
{
  BookmarkP me;
  char *tmp;

  me = (BookmarkP) TtaGetMemory (sizeof (Bookmark));
  memset (me, 0, sizeof (Bookmark));

  /* a q&d way for initializing the data, while waiting for dynamic string
     handling */
  if (isTopic)
    {
      me->parent_url = (char *)TtaGetMemory (MAX_LENGTH + 1);
      if (initial)
	{
	  if (initial->parent_url)
	    strcpy (me->parent_url, initial->parent_url);
	  else
	    me->parent_url[0] = EOS;
	}
      else
	strcpy (me->parent_url, GetHomeTopicURI ());
    }

  me->self_url = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->self_url)
    strcpy (me->self_url, initial->self_url);
  else
    me->self_url[0] = EOS;
  me->bookmarks = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->bookmarks)
    strcpy (me->bookmarks, initial->bookmarks);
  else
    me->bookmarks[0] = EOS;
  me->title = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->title)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->title, ISO_8859_1);
      strcpy (me->title, tmp);
      TtaFreeMemory (tmp);
    }
  else
    me->title[0] = EOS;
  me->author = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->author)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->author, ISO_8859_1);
      strcpy (me->author, tmp);
      TtaFreeMemory (tmp);
    }
  else
    me->author[0] = EOS;
  me->description = (char *)TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->description)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->description, ISO_8859_1);
      strcpy (me->description, tmp);
      TtaFreeMemory (tmp);
    }
  else
    me->description[0] = EOS;
  if (initial && initial->created)
    me->created = TtaStrdup (initial->created);
  else
    me->created = NULL;
  if (initial && initial->modified)
    me->modified = TtaStrdup (initial->modified);
  else
    me->modified = NULL;
  if (initial)
    me->isUpdate = initial->isUpdate;
  return (me);
}

/*----------------------------------------------------------------------
  BMMenu_new_init
  Intializes both the bm and dynbookmark structures.
  ----------------------------------------------------------------------*/
static void BMmenu_Bookmark_new_init (BookmarkP initial, ThotBool isTopic, BookmarkP *bm, 
				      dynBookmarkP *dbm)
{
  BookmarkP bme;
  dynBookmarkP dbme;
  char *tmp;

  /* the dynamic bookmark */
  dbme = (dynBookmarkP) TtaGetMemory (sizeof (dynBookmark));
  *dbm = dbme;

  memset (dbme, 0, sizeof (dynBookmark));

  if (isTopic)
    {
      dbme->parent_url = BM_bufferNew ();
      if (initial)
	{
	  if (initial->parent_url)
	    BM_bufferCopy (dbme->parent_url, initial->parent_url);
	}
      else
	BM_bufferCopy (dbme->parent_url, GetHomeTopicURI ());
    }

  dbme->self_url = BM_bufferNew ();
  if (initial && initial->self_url)
    BM_bufferCopy (dbme->self_url, initial->self_url);
  dbme->bookmarks = BM_bufferNew ();
  if (initial && initial->bookmarks)
    BM_bufferCopy (dbme->bookmarks, initial->bookmarks);
  dbme->title = BM_bufferNew ();
  if (initial && initial->title)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->title, ISO_8859_1);
      BM_bufferCopy (dbme->title, tmp);
      TtaFreeMemory (tmp);
    }
  dbme->author = BM_bufferNew ();
  if (initial && initial->author)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->author, ISO_8859_1);
      BM_bufferCopy (dbme->author, tmp);
      TtaFreeMemory (tmp);
    }
  dbme->description = BM_bufferNew ();
  if (initial && initial->description)
    {
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)initial->description, ISO_8859_1);
      BM_bufferCopy (dbme->description, tmp);
      TtaFreeMemory (tmp);
    }

  /* the normal bookmark */
  bme = (BookmarkP) TtaGetMemory (sizeof (Bookmark));
  *bm = bme;

  memset (bme, 0, sizeof (Bookmark));

  if (initial && initial->created)
    bme->created = TtaStrdup (initial->created);
  else
    bme->created = NULL;
  if (initial && initial->modified)
    bme->modified = TtaStrdup (initial->modified);
  else
    bme->modified = NULL;
  if (initial)
    bme->isUpdate = initial->isUpdate;
}

/*----------------------------------------------------------------------
  BMenu_bookmarkSynchronize
  Updates the pointers on a bookmark from the dynamic bookmark fields.
  ----------------------------------------------------------------------*/
static void BMenu_bookmarkSynchronize (dynBookmarkP dbme, BookmarkP bme)
{
  /* we link to the values of the dynamic bookmark */
  bme->parent_url = BM_bufferContent (dbme->parent_url);
  bme->self_url = BM_bufferContent (dbme->self_url);
  bme->bookmarks = BM_bufferContent (dbme->bookmarks);
  bme->title = BM_bufferContent (dbme->title);
  bme->author = BM_bufferContent (dbme->author);
  bme->description = BM_bufferContent (dbme->description);
}

/*----------------------------------------------------------------------
  BMenu_bookmarkclear
  Clears the Bookmark structure of the fields that are just copies
  from the dynamic structure one.
  ----------------------------------------------------------------------*/
static void BMenu_bookmarkclear (BookmarkP bme)
{

  bme->parent_url = NULL;
  bme->self_url = NULL;
  bme->bookmarks = NULL;
  bme->title = NULL;
  bme->author = NULL;
  bme->description = NULL;
}

/*----------------------------------------------------------------------
  BMenu_bookmarkFree
  Frees the memory allocated to a dbme and bme structure.
  ----------------------------------------------------------------------*/
static void BMmenu_bookmarkFree (dynBookmarkP *dbm, BookmarkP *bm)
{
  BookmarkP bme = *bm;
  dynBookmarkP dbme = *dbm;

  if (bme)
    {
      BMenu_bookmarkclear (bme);
      Bookmark_free (bme);
      *bm = NULL;
    }

  if (dbme)
    {
      BM_bufferFree (dbme->parent_url);
      BM_bufferFree (dbme->self_url);
      BM_bufferFree (dbme->bookmarks);
      BM_bufferFree (dbme->title);
      BM_bufferFree (dbme->author);
      BM_bufferFree (dbme->description);
      TtaFreeMemory (dbme);
      *dbm = NULL;
    }
}

/*----------------------------------------------------------------------
  ControlURIs
  Checks if the topic and bookmarks URLs are relative or absolute.
  ----------------------------------------------------------------------*/
static void ControlURIs (dynBookmarkP me)
{
  char *url;
  char *ptr;
  char *base;
  
  base = GetLocalBookmarksBaseURI ();

  url = BM_bufferContent (me->parent_url);
  if (url && url[0] && url[0] == '#')
    {
      ptr = Annot_ConcatenateBase (base, url);
      if (ptr)
	{
	  BM_bufferCopy (me->parent_url, ptr);
	  TtaFreeMemory (ptr);
	}
    }
  
  url = BM_bufferContent (me->self_url);
  if (url && url[0] && url[0] == '#')
    {
      ptr = Annot_ConcatenateBase (base, url);
      if (ptr)
	{
	  BM_bufferCopy (me->self_url, ptr);
	  TtaFreeMemory (ptr);
	}
    }
  
  url = BM_bufferContent (me->bookmarks);
  if (url && url[0] && !IsW3Path (url) && !IsFilePath (url))
    {
      char *ptr;
      ptr = ANNOT_MakeFileURL ((const char *) url);
      BM_bufferCopy (me->bookmarks, ptr);
      TtaFreeMemory (ptr);
    }
}

/*----------------------------------------------------------------------
  InitBookmarkMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void InitBookmarkMenu (Document doc, BookmarkP bookmark)
{
  char *ptr;
  char *ptr2;
  int   ref;
 BMmenu_Bookmark_new_init (bookmark, FALSE, &aBookmark, &aDynBookmark);
 if (bookmark)
   {
     /* if we have a bookmark pointer, it means we're viewing
	a bookmark file and examing the properties of one of 
	its items */
     BM_Context_reference (DocumentURLs[doc], &ref);
   }
 else
   ref = 0;
 aDynBookmark->ref = ref;
 aDynBookmark->doc = BM_getDocumentFromRef (ref);

 if (!bookmark)
    {
      if (!IsW3Path (DocumentURLs[doc])
	  && !IsW3Path (DocumentURLs[doc]))
	ptr = ANNOT_MakeFileURL (DocumentURLs[doc]);
      else 
	ptr = DocumentURLs[doc];

      if (DocumentMeta[doc] && DocumentMeta[doc]->form_data)
	{
	  char *ptr2;

	  ptr2 = (char *)TtaGetMemory (strlen (ptr) 
			       + strlen (DocumentMeta[doc]->form_data)
			       + 2);

	  sprintf (ptr2, "%s?%s", ptr, DocumentMeta[doc]->form_data);
	  if (ptr != DocumentURLs[doc])
	    TtaFreeMemory (ptr);
	  ptr = ptr2;
	}
      BM_bufferCopy (aDynBookmark->bookmarks, ptr);

      if (ptr != DocumentURLs[doc])
	TtaFreeMemory (ptr);

      ptr = ANNOT_GetHTMLTitle (doc);
      ptr2 = (char *)TtaConvertMbsToByte ((unsigned char *)ptr, ISO_8859_1);
      BM_bufferCopy (aDynBookmark->title, ptr2);
      TtaFreeMemory (ptr);
      TtaFreeMemory (ptr2);
      ptr = GetAnnotUser ();
      ptr2 = (char *)TtaConvertMbsToByte ((unsigned char *)ptr, ISO_8859_1);
      BM_bufferCopy (aDynBookmark->author, ptr2);
      TtaFreeMemory (ptr2);
      aBookmark->created = StrdupDate ();
      aBookmark->modified = StrdupDate ();
    }
}

/*----------------------------------------------------------------------
  RefreshBookmarkMenu
  Displays the current bookmark structure values in the menu
  ----------------------------------------------------------------------*/
static void RefreshBookmarkMenu ()
{
  char ** bm_urls = NULL;
  int count;
  ThotWidget combo;

  
  /* set the menu entries to the current values */

  /* count how many open bookmark files we have */
  /* get all the bookmark files */
  
  count = BM_Context_dumpAsCharList (&bm_urls);
  if (count) 
    {
      combo = TtaCatWidget (BookmarkBase + mBMBFileList);
      TtaInitComboBox (combo, count, bm_urls);
      TtaFreeMemory (bm_urls);
    }

  TtaSetTextForm (BookmarkBase + mBMTitle, BM_bufferContent (aDynBookmark->title));
  TtaSetTextForm (BookmarkBase + mBMBookmarks, BM_bufferContent (aDynBookmark->bookmarks));
  TtaSetTextForm (BookmarkBase + mBMAuthor, BM_bufferContent (aDynBookmark->author));
  TtaNewLabel (BookmarkBase + mBMCreated, BookmarkBase + BookmarkMenu, aBookmark->created);
  TtaNewLabel (BookmarkBase + mBMModified, BookmarkBase + BookmarkMenu, aBookmark->modified);
  TtaSetTextForm (BookmarkBase + mBMDescription, BM_bufferContent (aDynBookmark->description));
}

/*-----------------------------------------------------------------------
  Update the parent topic
  -----------------------------------------------------------------------*/
static void BookmarkMenuSelect_cbf (ThotWidget w, ThotBool state, void *cdata)
{
  char *url;
  Element el = (Element) cdata;

  url = BM_topicGetModelHref (el);
  if (url)
    {
      BM_topicSelectToggle (BTopicTree, url, state);
      TtaFreeMemory (url);
    }
}


/*----------------------------------------------------------------------
  BookmarkMenuCallbackDialog
  callback of the Bookmark menu
  ----------------------------------------------------------------------*/
static void BookmarkMenuCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;
  ThotBool             result;
  List                *parent_url_list;

  if (ref == -1)
    {
      /* removes the bookmark menu */
      TtaDestroyDialogue (BookmarkBase + BookmarkMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - BookmarkBase)
	{
	case BookmarkMenu:
	  switch (val) 
	    {
	    case 0: /* done */
	      BMmenu_bookmarkFree (&aDynBookmark, &aBookmark);
	      TtaCloseDocument (BTopicTree);
	      BTopicTree = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create bookmark */
	      ControlURIs (aDynBookmark);
	      BMenu_bookmarkSynchronize (aDynBookmark, aBookmark);
	      BM_dumpTopicTreeSelections (BTopicTree, &parent_url_list);
	      if (parent_url_list)
		{
		  if (aBookmark->parent_url_list)
		    {
		      List *tmp;
		      tmp = aBookmark->parent_url_list;
		      List_delAll (&tmp, (ThotBool (*)(void*)) TtaFreeMemory);
		    }
		  aBookmark->parent_url_list = parent_url_list;
		  /* @@ JK: Editing only the local file with these menus */
		  if (aBookmark->isUpdate) 
		    result = BM_updateItem (aDynBookmark->doc,
					    aDynBookmark->ref, aBookmark, FALSE);
		  else
		    result = BM_addBookmark (aDynBookmark->doc, 
					     aDynBookmark->ref, aBookmark);
		  if (result)
		    {
		      /* if successful, close the dialog */
		      TtaCloseDocument (BTopicTree);
		      BTopicTree = 0;

		      /* refresh the view */
		      BM_refreshBookmarkView (aDynBookmark->ref);
		      BMmenu_bookmarkFree (&aDynBookmark, &aBookmark);
		      TtaDestroyDialogue (ref);
		    }
		}
	      break;
	    case 2: /* new topic */
	      /* @@ JK: we force them to the local one, but we 
	       need more context to give the good ref. */
	      BM_TopicMenu (aDynBookmark->doc, 1, aDynBookmark->ref, NULL);
	      break;

	    default:
	      break;
	    }
	  break;

	case mBMBFileList:
	  if (typedata == STRING_DATA && data && data[0])
	    {
	      int ref; 
	      ThotBool found;

	      if (!strcmp (data, "Default bookmark file"))
		{
		  ref = 0;
		  found = TRUE;
		}
	      else 
		found = BM_Context_reference (data, &ref);

	      if (found)
		{
		  aDynBookmark->ref = ref;
		  /* change the doc so that we update the correct view */
		  aDynBookmark->doc =  BM_getDocumentFromRef (ref);
		  BM_RefreshTopicTree (ref);
		}
	      else
		printf ("uh oh, no ref\n");
	    }
	  break;

	case mBMTitle:
	  if (data)
	    BM_bufferCopy (aDynBookmark->title, data);
	  else
	    BM_bufferClear (aDynBookmark->title);
	  break;

	case mBMBookmarks:
	  if (data)
	    BM_bufferCopy (aDynBookmark->bookmarks, data);
	  else
	    BM_bufferClear (aDynBookmark->bookmarks);
	  break;

	case mBMAuthor:
	  if (data)
	    BM_bufferCopy (aDynBookmark->author, data);
	  else
	    BM_bufferClear (aDynBookmark->author);
	  break;

	case mBMDescription:
	  if (data)
	    BM_bufferCopy (aDynBookmark->description, data);
	  else
	    BM_bufferClear (aDynBookmark->description);
	  break;

	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
  BM_BookmarkMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void BM_BookmarkMenu (Document doc, View view, int ref, BookmarkP bookmark)
{
#ifndef _WINGUI
   int i;
   ThotWidget tree;
   char *label;
   int longest_label;

   /* compute the longest label */
   longest_label = strlen (TtaGetMessage (AMAYA, AM_BM_TOPIC_HIERARCHY));
   i = strlen (TtaGetMessage (AMAYA, AM_BM_MODIFIED));
   if (i > longest_label)
     longest_label = i;
   longest_label++;

   if (BookmarkBase == 0)
     BookmarkBase = TtaSetCallback ((Proc)BookmarkMenuCallbackDialog, MAX_BOOKMARKMENU_DLG);
   
   if (aDynBookmark && aBookmark)
     BMmenu_bookmarkFree (&aDynBookmark, &aBookmark);

   /* create the new bookmark structure */
   InitBookmarkMenu (doc, bookmark);

   /* dump the topics as a thot three */
   if (BTopicTree != 0)
     TtaCloseDocument (BTopicTree);
   BTopicTree = BM_GetTopicTree (ref);
   
   /* select the topics in the tree that correspond to those in the bookmark */
   BM_topicsPreSelect (ref, BTopicTree, bookmark);

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i = strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_NEW_TOPIC));

   TtaNewSheet (BookmarkBase + BookmarkMenu, 
		TtaGetViewFrame (doc, view),
		TtaGetMessage (AMAYA, AM_BM_BPROPERTIES),
		3, s, TRUE, 2, 'L', D_DONE);

   if (MultipleBookmarks () && !bookmark)
     {
       /* we only show this property for new bookmarks */
       /* bookmark files */
       TtaNewPaddedLabel (BookmarkBase + mBMBFileListL,  BookmarkBase + BookmarkMenu, 
			  "Bookmark store", longest_label);
       TtaNewComboBox (BookmarkBase + mBMBFileList,
		       BookmarkBase + BookmarkMenu,
		       NULL, TRUE);
     }

   /* topic hiearchy */
   label = TtaGetMessage (AMAYA, AM_BM_TOPIC_HIERARCHY);
   TtaNewPaddedLabel (BookmarkBase + mBMTopicTreeL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);

   tree = TtaNewTreeForm (BookmarkBase + mBMTopicTree,
			   BookmarkBase + BookmarkMenu,
			   NULL,
			   TRUE,
			   (void *) BookmarkMenuSelect_cbf);
   if (tree)
     BM_InitTreeWidget (tree, BTopicTree);

   /* Title */
   label = TtaGetMessage (AMAYA, AM_BM_TITLE);
   TtaNewPaddedLabel (BookmarkBase + mBMTitleL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewTextForm (BookmarkBase + mBMTitle,
		   BookmarkBase + BookmarkMenu,
		   NULL,
		   30,
		   1,
		   TRUE);

   /* Recalls */
   label = TtaGetMessage (AMAYA, AM_BM_RECALLS);
   TtaNewPaddedLabel (BookmarkBase + mBMBookmarksL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewTextForm (BookmarkBase + mBMBookmarks,
		   BookmarkBase + BookmarkMenu,
		   NULL,
		   30,
		   1,
		   TRUE);
   /* Author */
   label = TtaGetMessage (AMAYA, AM_BM_AUTHOR);
   TtaNewPaddedLabel (BookmarkBase + mBMAuthorL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewTextForm (BookmarkBase + mBMAuthor,
		   BookmarkBase + BookmarkMenu,
		   NULL,
		   30,
		   1,
		   TRUE);

   /* Created */
   label = TtaGetMessage (AMAYA, AM_BM_CREATED);
   TtaNewPaddedLabel (BookmarkBase + mBMCreatedL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewLabel (BookmarkBase + mBMCreated,  BookmarkBase + BookmarkMenu, 
		aBookmark->created);

   /* Modified */
   label = TtaGetMessage (AMAYA, AM_BM_MODIFIED);
   TtaNewPaddedLabel (BookmarkBase + mBMModifiedL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewLabel (BookmarkBase + mBMModified,  BookmarkBase + BookmarkMenu, 
		aBookmark->modified);

   /* Description */
   label = TtaGetMessage (AMAYA, AM_BM_DESCRIPTION);
   TtaNewPaddedLabel (BookmarkBase + mBMDescriptionL,  BookmarkBase + BookmarkMenu, 
		      label, longest_label);
   TtaNewTextForm (BookmarkBase + mBMDescription,
		   BookmarkBase + BookmarkMenu,
		   NULL,
		   30,
		   5,
		   TRUE);
#endif /* !_WINGUI */

#ifndef _WINGUI
   RefreshBookmarkMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (BookmarkBase + BookmarkMenu, TRUE, TRUE);
#else
#if 0
   /* @@@ JK: To be programmed in win32 */
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL, (DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif
#endif /* !_WINGUI */
}

/*----------------------------------------------------------------------
  InitTopicMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void InitTopicMenu (Document doc, BookmarkP bookmark)
{
  char *annotUser;
  char *tmp;
  int   ref;

  BMmenu_Bookmark_new_init (bookmark, TRUE, &aTopic, &aDynTopic);
  aTopic->bm_type = BME_TOPIC;

 if (bookmark)
   {
     /* if we have a bookmark pointer, it means we're viewing
	a bookmark file and examing the properties of one of 
	its items */
     BM_Context_reference (DocumentURLs[doc], &ref);
   }
 else if (DocumentTypes[doc] == docBookmark)
     BM_Context_reference (DocumentURLs[doc], &ref);     
 else
   ref = 0;

  aDynTopic->ref = ref;
  aDynTopic->doc = BM_getDocumentFromRef (ref);

  if (!bookmark)
    {
      annotUser = GetAnnotUser ();
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)annotUser, ISO_8859_1);
      BM_bufferCopy (aDynTopic->author, tmp);
      TtaFreeMemory (tmp);
      aTopic->created = StrdupDate ();
      aTopic->modified = StrdupDate ();
    }
}

/*----------------------------------------------------------------------
  RefreshTopicMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshTopicMenu ()
{
  char ** bm_urls = NULL;
  int count;
  ThotWidget combo;

  /* set the menu entries to the current values */

  /* count how many open bookmark files we have */
  /* get all the bookmark files */
  
  count = BM_Context_dumpAsCharList (&bm_urls);
  if (count) 
    {
      combo = TtaCatWidget (TopicBase + mTMBFileList);
      TtaInitComboBox (combo, count, bm_urls);
      TtaFreeMemory (bm_urls);
    }

  /* TtaSetTextForm (TopicBase + mTMParentTopic, BM_bufferContent (aDynTopic->parent_url)); */
  TtaSetTextForm (TopicBase + mTMTitle, BM_bufferContent (aDynTopic->title));
  TtaSetTextForm (TopicBase + mTMAuthor, BM_bufferContent (aDynTopic->author));
  TtaSetTextForm (TopicBase + mTMDescription, BM_bufferContent (aDynTopic->description));
}

/*-----------------------------------------------------------------------
  Update the parent topic
  -----------------------------------------------------------------------*/
static void TopicMenuSelect_cbf (ThotWidget w, ThotBool state, void *cdata)
{
  char *url;
  Element el = (Element) cdata;

  url = BM_topicGetModelHref (el);
  
  if (url)
    {
      BM_bufferCopy (aDynTopic->parent_url, url);
      /* @@ JK: just for debugging */
      /* TtaSetTextForm (TopicBase + mTMParentTopic, BM_bufferContent (aDynTopic->parent_url)); */
      TtaFreeMemory (url);
    }
}

/*----------------------------------------------------------------------
  TopicMenuCallbackDialog
  callback of the Topic menu
  ----------------------------------------------------------------------*/
static void TopicMenuCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;
  int                 result;

  if (ref == -1)
    {
      /* removes the topic menu */
      TtaDestroyDialogue (TopicBase + TopicMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - TopicBase)
	{
	case TopicMenu:
	  switch (val) 
	    {
	    case 0: /* done */
	      BMmenu_bookmarkFree (&aDynTopic, &aTopic);
	      TtaCloseDocument (TTopicTree);
	      TTopicTree = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create topic */
	      ControlURIs (aDynTopic);
	      BMenu_bookmarkSynchronize (aDynTopic, aTopic);
	      /* @@ JK: Editing only the local file with these menus */
	      if (aTopic->isUpdate)
		result = BM_updateItem (aDynTopic->doc, aDynTopic->ref,
					aTopic, TRUE);
	      else
		result = BM_addTopic (aDynTopic->doc, aDynTopic->ref,
				      aTopic, TRUE);
	      if (result)
		{
		  Document a_doc;
		  int      a_ref;

		  /* if successful, close the dialog */
		  a_doc = aDynTopic->doc;
		  a_ref = aDynTopic->ref;
		  BMmenu_bookmarkFree (&aDynTopic, &aTopic);
		  TtaCloseDocument (TTopicTree);
		  TTopicTree = 0;
		  /* refresh the topic view */
		  BM_refreshBookmarkView (a_ref);
		  /* and the open widgets */
		  BM_RefreshTopicTree (a_ref);
		  TtaDestroyDialogue (ref);
		}
	      break;

	    default:
	      break;
	    }
	  break;

	case mTMParentTopic:
	  if (data)
	    BM_bufferCopy (aDynTopic->parent_url, data);
	  else
	    BM_bufferClear (aDynTopic->parent_url);
	  break;

	case mTMBFileList:
	  if (typedata == STRING_DATA && data && data[0])
	    {
	      int ref; 
	      ThotBool found;
	      
	      if (!strcmp (data, "Default bookmark file"))
		{
		  ref = 0;
		  found = TRUE;
		}
	      else 
		found = BM_Context_reference (data, &ref);
	      
	      if (found)
		{
		  aDynTopic->ref = ref;
		  /* change the doc so that we update the correct view */
		  aDynTopic->doc =  BM_getDocumentFromRef (ref);
		  BM_RefreshTopicTree (ref);
		}
	      else
		printf ("uh oh, no ref\n");
	    }
	  break;

	case mTMTitle:
	  if (data)
	    BM_bufferCopy (aDynTopic->title, data);
	  else
	    BM_bufferClear (aDynTopic->title);
	  break;

	case mTMAuthor:
	  if (data)
	    BM_bufferCopy (aDynTopic->author, data);
	  else
	    BM_bufferClear (aDynTopic->author);
	  break;

	case mTMDescription:
	  if (data)
	    BM_bufferCopy (aDynTopic->description, data);
	  else
	    BM_bufferClear (aDynTopic->description);
	  break;

	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
  BM_TopicMenu
  Build and display the Conf Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void BM_TopicMenu (Document doc, View view, int ref, BookmarkP bookmark)
{
#ifndef _WINGUI
   int i;
   char *label;
   int longest_label;
   ThotWidget tree;

   /* compute the longest label */
   longest_label = strlen (TtaGetMessage (AMAYA, AM_BM_TOPIC_HIERARCHY));
   i = strlen (TtaGetMessage (AMAYA, AM_BM_MODIFIED));
   if (i > longest_label)
     longest_label = i;
   longest_label++;

   if (TopicBase == 0)
     TopicBase = TtaSetCallback ((Proc)TopicMenuCallbackDialog, MAX_TOPICMENU_DLG);
  
   if (aTopic && aDynTopic)
     BMmenu_bookmarkFree (&aDynTopic, &aTopic);

   /* make the new topic */
   InitTopicMenu (doc, bookmark);

   ref = aDynTopic->ref;

   /* dump the topics as a thot three */
   if (TTopicTree != 0)
     TtaCloseDocument (TTopicTree);
   TTopicTree = BM_GetTopicTree (ref);

   /* remove the child entries. We cannot move a topic to its children */
   BM_topicsPrune (TTopicTree, bookmark);

   /* select the topics in the tree that correspond to those in the bookmark */
   BM_topicsPreSelect (ref, TTopicTree, bookmark);

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));

   TtaNewSheet (TopicBase + TopicMenu, 
		TtaGetViewFrame (doc, view),
		TtaGetMessage (AMAYA, AM_BM_TPROPERTIES),
		1, s, TRUE, 2, 'L', D_DONE);

   if (MultipleBookmarks () && !bookmark)
     {
       /* we only show this property for new bookmarks */
       /* bookmark files */
       TtaNewPaddedLabel (TopicBase + mTMBFileListL,  TopicBase + TopicMenu, 
			  "Bookmark store", longest_label);
       TtaNewComboBox (TopicBase + mTMBFileList,
		       TopicBase + TopicMenu,
		       NULL, TRUE);
     }

   /* topic hierarchy */
   label = TtaGetMessage (AMAYA, AM_BM_TOPIC_HIERARCHY);
   TtaNewPaddedLabel (TopicBase + mTMTopicTreeL,  TopicBase + TopicMenu,
		      label, longest_label);
   tree = TtaNewTreeForm (TopicBase + mTMTopicTree,
			  TopicBase + TopicMenu,
			  NULL,
			  FALSE,
			  (void *) TopicMenuSelect_cbf);
   if (tree)
     BM_InitTreeWidget (tree, TTopicTree);

#if 0 
   /* parent topic */
   label = "Parent Topic:";
   TtaNewPaddedLabel (TopicBase + mTMParentTopicL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewTextForm (TopicBase + mTMParentTopic,
		   TopicBase + TopicMenu,
		   NULL,
		   30,
		   1,
		   TRUE);
#endif

   /* topic title */
   label = TtaGetMessage (AMAYA, AM_BM_TITLE);
   TtaNewPaddedLabel (TopicBase + mTMTitleL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewTextForm (TopicBase + mTMTitle,
		   TopicBase + TopicMenu,
		   NULL, 
		   30,
		   1,
		   TRUE);

   /* author */
   label = TtaGetMessage (AMAYA, AM_BM_AUTHOR);
   TtaNewPaddedLabel (TopicBase + mTMAuthorL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewTextForm (TopicBase + mTMAuthor,
		   TopicBase + TopicMenu,
		   NULL,
		   30,
		   1,
		   TRUE);

   /* created */
   label = TtaGetMessage (AMAYA, AM_BM_CREATED);
   TtaNewPaddedLabel (TopicBase + mTMCreatedL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewLabel (TopicBase + mTMCreated, TopicBase + TopicMenu,
		aTopic->created);

   /* last modified */
   label = TtaGetMessage (AMAYA, AM_BM_MODIFIED);
   TtaNewPaddedLabel (TopicBase + mTMModifiedL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewLabel (TopicBase + mTMModified, TopicBase + TopicMenu,
		aTopic->modified);

   /* description */
   label =    TtaGetMessage (AMAYA, AM_BM_DESCRIPTION);
   TtaNewPaddedLabel (TopicBase + mTMDescriptionL,  TopicBase + TopicMenu,
		      label, longest_label);
   TtaNewTextForm (TopicBase + mTMDescription,
		   TopicBase + TopicMenu,
		   NULL,
		   30,
		   5,
		   TRUE);
#endif /* !_WINGUI */

#ifndef _WINGUI
   RefreshTopicMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (TopicBase + TopicMenu, TRUE, TRUE);
#else
#if 0
   /* @@@ JK: To be programmed in win32 */
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL, (DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif
#endif /* !_WINGUI */
}


/*----------------------------------------------------------------------
  TopicURLCallbackDialog
  callback of the Topic menu
  ----------------------------------------------------------------------*/
static void TopicURLCallbackDialog (int ref, int typedata, char *data)
{
  intptr_t            val;
  char                tempname[MAX_LENGTH];
  char                tempfile[MAX_LENGTH];
  if (ref == -1)
    {
      /* removes the bookmark menu */
      TtaDestroyDialogue (TopicURLBase + TopicURLMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - TopicURLBase)
	{
	case TopicURLMenu:
	  switch (val) 
	    {
	    case 0: /* cancel */
	      LastURLTopic[0] = EOS;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* confirm */
	      /* ControlURIs (LastURLTopic); */
	      if (LastURLTopic[0] != EOS)
		TtaDestroyDialogue (ref);
	      break;
	    case 2: /* clear */
	      LastURLTopic[0] = EOS;
	      TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
	      TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
	      break;
	    case 3: /* filter button */
	      /* reinitialize directories and document lists */
	      TtaListDirectory (TopicDirectory, ref,
				"Topic directory",
				TopicURLBase + mTUMDir,
				TopicFilter,
				TtaGetMessage (AMAYA, AM_FILES), 
				TopicURLBase + mTUMSel);
	      break;

	    default:
	      break;
	    }
	  break;

	case mTUMDir:
	  if (!strcmp (data, ".."))
	    {
	      /* suppress last directory */
	      strcpy (tempname, TopicDirectory);
	      TtaExtractName (tempname, TopicDirectory, tempfile);
	    }
	  else
	    {
	      strcat (TopicDirectory, DIR_STR);
	      strcat (TopicDirectory, data);
	    }
#ifndef _WINGUI
	  TtaSetTextForm (TopicURLBase + mTUMURL, TopicDirectory);
#endif /* !_WINGUI */
	  TtaListDirectory (TopicDirectory, 
			    TopicURLBase + TopicURLMenu,
			    TtaGetMessage (LIB, TMSG_DOC_DIR), 
			    TopicURLBase + mTUMDir,
			    TopicFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    TopicURLBase + mTUMSel);
	  break;

	case mTUMSel:
	  if (TopicDirectory[0] == EOS)
      {
        /* set path on current directory */
        char *ptr = TtaGetEnvString ("cwd");
        if (ptr)
          strncpy (TopicDirectory, ptr, MAX_LENGTH);
      }
	  /* construct the full name of the topic */
	  strcpy (LastURLTopic, TopicDirectory);
	  val = strlen (LastURLTopic) - 1;
	  if (LastURLTopic[val] != DIR_SEP)
	    strcat (LastURLTopic, DIR_STR);
	  strcat (LastURLTopic, data);
#ifndef _WINGUI
	  TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
#endif /* !_WINGUI */
	  break;

    case mTUMFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (TopicFilter, data);
#ifndef _WINGUI
      else
	TtaSetTextForm (TopicURLBase + mTUMFilter, TopicFilter);
#endif /* !_WINGUI */
      break;

	default:
	  break;

	}
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitTopicURL (void)
{
  char *ptr = TtaGetEnvString ("cwd");

   /* set path on current directory */
  if (ptr)
    strncpy (TopicDirectory, ptr, MAX_LENGTH);
   LastURLTopic[0] = EOS;
   strcpy (TopicFilter, "*.rdf");
}

/*----------------------------------------------------------------------
  GetTopicURL Gets the URL of a topic hierarchy to download
  ----------------------------------------------------------------------*/
char *GetTopicURL (Document document, View view)
{
#ifndef _WINGUI
   char               s[MAX_LENGTH];
   int                 i;

   if (TopicURLBase == 0)
     TopicURLBase = TtaSetCallback ((Proc)TopicURLCallbackDialog, MAX_TOPICURLMENU_DLG);

   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (TopicURLBase + TopicURLMenu, 
		TtaGetViewFrame (document, view),
		"Import Topic hierarchy",
		3, s, TRUE, 2, 'L', D_CANCEL);

   TtaNewTextForm (TopicURLBase + mTUMURL, 
		   TopicURLBase + TopicURLMenu, 
		   "URL:", 50, 1, FALSE);
   TtaNewLabel (TopicURLBase + mTUML1, TopicURLBase + TopicURLMenu, " ");

   TtaListDirectory (TopicDirectory, 
		     TopicURLBase + TopicURLMenu,
		     TtaGetMessage (LIB, TMSG_DOC_DIR), 
		     TopicURLBase + mTUMDir, 
		     TopicFilter,
		     TtaGetMessage (AMAYA, AM_FILES), 
		     TopicURLBase + mTUMSel);

   if (LastURLTopic[0] != EOS)
      TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
   else
     {
	strcpy (LastURLTopic, TopicDirectory);
	strcat (LastURLTopic, DIR_STR);
	TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
     }

   TtaNewTextForm (TopicURLBase + mTUMFilter, TopicURLBase + TopicURLMenu,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (TopicURLBase + mTUMFilter, TopicFilter);
   TtaNewLabel (TopicURLBase + mTUML2, TopicURLBase + TopicURLMenu, " ");

   TtaSetDialoguePosition ();

   TtaShowDialogue (TopicURLBase + TopicURLMenu, FALSE, TRUE);
   TtaWaitShowDialogue ();

#endif /* _WINGUI */
   return (LastURLTopic);

}

/*----------------------------------------------------------------------
  BM_RefreshTopicTree
  Redisplays the topic tree in the topic and bookmark widgets, if they
  are open.
  ----------------------------------------------------------------------*/
void BM_RefreshTopicTree (int ref)
{
  ThotWidget tree;

  if (aBookmark && aDynBookmark->ref == ref)
    {
      tree = TtaCatWidget (BookmarkBase + mBMTopicTree);
      tree = TtaClearTree (tree);
      if (BTopicTree)
	TtaCloseDocument (BTopicTree);
      BTopicTree = BM_GetTopicTree (ref);
      /* select the topics in the tree that correspond to those in the bookmark */
      aBookmark->self_url = BM_bufferContent (aDynBookmark->self_url);
      BM_topicsPreSelect (ref, BTopicTree, aBookmark);
      aBookmark->self_url = NULL;
      BM_InitTreeWidget (tree, BTopicTree);
    }

  if (aTopic && aDynTopic->ref == ref )
    {
      tree = TtaCatWidget (TopicBase + mTMTopicTree);
      if (tree)
	{
	  tree = TtaClearTree (tree);
	  if (TTopicTree)
	    TtaCloseDocument (TTopicTree);
	  TTopicTree = BM_GetTopicTree (ref);
	  /* select the topics in the tree that correspond to those in the bookmark */
	  aTopic->self_url = BM_bufferContent (aDynTopic->self_url);
	  aTopic->parent_url = BM_bufferContent (aDynTopic->parent_url);
	  BM_topicsPreSelect (ref, TTopicTree, aTopic);
	  aTopic->self_url = NULL;
	  aTopic->parent_url = NULL;
	  BM_InitTreeWidget (tree, TTopicTree);
	}
    }
}

