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

/* amaya includes */
#include "AHTURLTools_f.h"

static char    s[MAX_LENGTH]; /* general purpose buffer */

static int       BookmarkBase;
static int       TopicBase;
static BookmarkP aBookmark;

/*----------------------------------------------------------------------
  Bookmark_new_init
  ----------------------------------------------------------------------*/
BookmarkP Bookmark_new_init (BookmarkP initial)
{
  BookmarkP me;

  me = (BookmarkP) TtaGetMemory (sizeof (Bookmark));
  memset (me, 0, sizeof (Bookmark));

  /* a q&d way for initializing the data, while waiting for dynamic string
     handling */
  me->parent_url = TtaGetMemory (MAX_LENGTH + 1);
  if (initial)
    {
      if (initial->parent_url)
	strcpy (me->parent_url, initial->parent_url);
      else
	me->parent_url[0] = EOS;
    }
  else
    strcpy (me->parent_url, GetHomeTopicURI ());
  me->self_url = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->self_url)
    strcpy (me->self_url, initial->self_url);
  else
    me->self_url[0] = EOS;
  me->bookmarks = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->bookmarks)
    strcpy (me->bookmarks, initial->bookmarks);
  else
    me->bookmarks[0] = EOS;
  me->title = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->title)
    strcpy (me->title, initial->title);
  else
    me->title[0] = EOS;
  me->author = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->author)
    strcpy (me->author, initial->author);
  else
    me->author[0] = EOS;
  me->description = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->description)
    strcpy (me->description, initial->description);
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
  ControlURIs
  Checks if the topic and bookmarks URLs are relative or absolute.
  ----------------------------------------------------------------------*/
static void ControlURIs (BookmarkP me)
{
  char *ptr;
  char *base;
  
  base = GetLocalBookmarksBaseURI ();

  if (me->parent_url && me->parent_url[0] && me->parent_url[0] == '#')
    {
      ptr = Annot_ConcatenateBase (base, me->parent_url);
      if (ptr)
	{
	  TtaFreeMemory (me->parent_url);
	  me->parent_url = ptr;
	}
    }
  if (me->self_url && me->self_url[0] && me->self_url[0] == '#')
    {
      ptr = Annot_ConcatenateBase (base, me->self_url);
      if (ptr)
	{
	  TtaFreeMemory (me->self_url);
	  me->self_url = ptr;
	}
    }
  if (me->bookmarks && me->bookmarks[0] && !IsW3Path (me->bookmarks) 
      && !IsFilePath (me->bookmarks))
    {
      char *ptr;
      ptr = ANNOT_MakeFileURL ((const char *) me->bookmarks);
      TtaFreeMemory (me->bookmarks);
      me->bookmarks = ptr;
    }
}

/*----------------------------------------------------------------------
  InitBookmarkMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void InitBookmarkMenu (Document doc, BookmarkP bookmark)
{
  char *ptr;

  aBookmark = Bookmark_new_init (bookmark);

  if (!bookmark)
    {
      if (!IsW3Path (DocumentURLs[doc])
	  && !IsW3Path (DocumentURLs[doc]))
	ptr = ANNOT_MakeFileURL (DocumentURLs[doc]);
      else 
	ptr = DocumentURLs[doc];
      strcpy (aBookmark->bookmarks, ptr);
      if (ptr != DocumentURLs[doc])
	TtaFreeMemory (ptr);
      strcpy (aBookmark->title, ANNOT_GetHTMLTitle (doc));
      strcpy (aBookmark->author, GetAnnotUser ());
      aBookmark->created = StrdupDate ();
      aBookmark->modified = StrdupDate ();
    }
}

/*----------------------------------------------------------------------
  RefreshBookmarkMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshBookmarkMenu ()
{
  /* set the menu entries to the current values */
  TtaSetTextForm (BookmarkBase + mBMTopic, aBookmark->parent_url);
  TtaSetTextForm (BookmarkBase + mBMTitle, aBookmark->title);
  TtaSetTextForm (BookmarkBase + mBMBookmarks, aBookmark->bookmarks);
  TtaSetTextForm (BookmarkBase + mBMAuthor, aBookmark->author);
  TtaSetTextForm (BookmarkBase + mBMCreated, aBookmark->created);
  TtaSetTextForm (BookmarkBase + mBMModified, aBookmark->modified);
  TtaSetTextForm (BookmarkBase + mBMDescription, aBookmark->description);
}

/*----------------------------------------------------------------------
  BookmarkMenuCallbackDialog
  callback of the Bookmark menu
  ----------------------------------------------------------------------*/
static void BookmarkMenuCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;
  ThotBool            result;

  if (ref == -1)
    {
      /* removes the cache conf menu */
      TtaDestroyDialogue (BookmarkBase + BookmarkMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - BookmarkBase)
	{
	case BookmarkMenu:
	  switch (val) 
	    {
	    case 0: /* done */
	      Bookmark_free (aBookmark);
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create bookmark */
	      ControlURIs (aBookmark);
	      if (aBookmark->isUpdate)
		result = BM_updateItem (aBookmark, FALSE);
	      else
		result = BM_addBookmark (aBookmark);
	      if (result)
		{
		  /* if successful, close the dialog */
		  Bookmark_free (aBookmark);
		  TtaDestroyDialogue (ref);
		}
	      break;
	    case 2: /* browse */
	      Bookmark_free (aBookmark);
	      TtaDestroyDialogue (ref);
	      break;
	    case 3:
	      break;

	    default:
	      break;
	    }
	  break;

	case mBMTopic:
	  if (data)
	    strcpy (aBookmark->parent_url, data);
	  else
	    aBookmark->parent_url[0] = EOS;
	  break;

	case mBMTitle:
	  if (data)
	    strcpy (aBookmark->title, data);
	  else
	    aBookmark->title[0] = EOS;
	  break;

	case mBMBookmarks:
	  if (data)
	    strcpy (aBookmark->bookmarks, data);
	  else
	    aBookmark->bookmarks[0] = EOS;
	  break;

	case mBMAuthor:
	  if (data)
	    strcpy (aBookmark->author, data);
	  else
	    aBookmark->author[0] = EOS;
	  break;

	case mBMDescription:
	  if (data)
	    strcpy (aBookmark->description, data);
	  else
	    aBookmark->description[0] = EOS;
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
void BM_BookmarkMenu (Document doc, View view, BookmarkP bookmark)
{
#ifndef _WINDOWS
   int i;

   if (BookmarkBase == 0)
     BookmarkBase = TtaSetCallback (BookmarkMenuCallbackDialog, MAX_BOOKMARKMENU_DLG);
  
   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));

   TtaNewSheet (BookmarkBase + BookmarkMenu, 
		TtaGetViewFrame (doc, view),
		"Bookmark Properties",
		2, s, FALSE, 8, 'L', D_DONE);

#if 0
   TtaNewScrollPopup (BookmarkBase +mBMTopic,
		      BookmarkBase + BookmarkMenu,
		      "Topic",
		      1,
		      "To do",
		      FALSE,
		      'L')

   TtaNewTextForm (BookmarkBase + mBMParentTopic,
		   BookmarkBase + BookmarkMenu,
		   "Parent Topic:",
		   30,
		   1,
		   TRUE);
#endif

   TtaNewTextForm (BookmarkBase + mBMTopic,
		   BookmarkBase + BookmarkMenu,
		   "Topic:",
		   30,
		   1,
		   TRUE);

   TtaNewTextForm (BookmarkBase + mBMTitle,
		   BookmarkBase + BookmarkMenu,
		   "Title:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (BookmarkBase + mBMBookmarks,
		   BookmarkBase + BookmarkMenu,
		   "URL:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (BookmarkBase + mBMAuthor,
		   BookmarkBase + BookmarkMenu,
		   "Author:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (BookmarkBase + mBMCreated,
		   BookmarkBase + BookmarkMenu,
		   "Created:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (BookmarkBase + mBMModified,
		   BookmarkBase + BookmarkMenu,
		   "Last Modified:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (BookmarkBase + mBMDescription,
		   BookmarkBase + BookmarkMenu,
		   "Description:",
		   30,
		   5,
		   TRUE);
#endif /* !_WINDOWS */

#if 0
   /* load and display the current values */
   GetAnnotConf ();
#endif

   InitBookmarkMenu (doc, bookmark);
#ifndef _WINDOWS
   RefreshBookmarkMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (BookmarkBase + BookmarkMenu, TRUE);
#else
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL, (DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  InitTopicMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void InitTopicMenu (Document doc, BookmarkP bookmark)
{
  aBookmark = Bookmark_new_init (bookmark);
  aBookmark->isTopic = TRUE;

  if (!bookmark)
    {
      strcpy (aBookmark->author, GetAnnotUser ());
      aBookmark->created = StrdupDate ();
      aBookmark->modified = StrdupDate ();
    }
}

/*----------------------------------------------------------------------
  RefreshTopicMenu
  Displays the current registry values in the menu
  ----------------------------------------------------------------------*/
static void RefreshTopicMenu ()
{
  /* set the menu entries to the current values */
  TtaSetTextForm (TopicBase + mTMParentTopic, aBookmark->parent_url);
  TtaSetTextForm (TopicBase + mTMTopicURL, aBookmark->self_url);
  TtaSetTextForm (TopicBase + mTMTitle, aBookmark->title);
  TtaSetTextForm (TopicBase + mTMAuthor, aBookmark->author);
  TtaSetTextForm (TopicBase + mTMCreated, aBookmark->created);
  TtaSetTextForm (TopicBase + mTMModified, aBookmark->modified);
  TtaSetTextForm (TopicBase + mTMDescription, aBookmark->description);
}

/*----------------------------------------------------------------------
  TopicMenuCallbackDialog
  callback of the Topic menu
  ----------------------------------------------------------------------*/
static void TopicMenuCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;
  int                 result;

  if (ref == -1)
    {
      /* removes the cache conf menu */
      TtaDestroyDialogue (TopicBase + TopicMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - TopicBase)
	{
	case TopicMenu:
	  switch (val) 
	    {
	    case 0: /* done */
	      Bookmark_free (aBookmark);
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create topic */
	      ControlURIs (aBookmark);
	      if (aBookmark->isUpdate)
		result = BM_updateItem (aBookmark, TRUE);
	      else
		result = BM_addTopic (aBookmark, TRUE);
	      if (result)
		{
		  /* if successful, close the dialog */
		  Bookmark_free (aBookmark);
		  TtaDestroyDialogue (ref);
		}
	      break;

	    default:
	      break;
	    }
	  break;

	case mTMParentTopic:
	  if (data)
	    strcpy (aBookmark->parent_url, data);
	  else
	    aBookmark->parent_url[0] = EOS;
	  break;

	case mTMTopicURL:
	  if (data)
	    strcpy (aBookmark->self_url, data);
	  else
	    aBookmark->self_url[0] = EOS;
	  break;

	case mTMTitle:
	  if (data)
	    strcpy (aBookmark->title, data);
	  else
	    aBookmark->title[0] = EOS;
	  break;

	case mTMAuthor:
	  if (data)
	    strcpy (aBookmark->author, data);
	  else
	    aBookmark->author[0] = EOS;
	  break;

	case mTMDescription:
	  if (data)
	    strcpy (aBookmark->description, data);
	  else
	    aBookmark->description[0] = EOS;
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
void BM_TopicMenu (Document doc, View view, BookmarkP bookmark)
{
#ifndef _WINDOWS
   int i;

   if (TopicBase == 0)
     TopicBase = TtaSetCallback (TopicMenuCallbackDialog, MAX_TOPICMENU_DLG);
  
   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));

   TtaNewSheet (TopicBase + TopicMenu, 
		TtaGetViewFrame (doc, view),
		"Topic Properties",
		1, s, FALSE, 8, 'L', D_DONE);

#if 0
   TtaNewScrollPopup (TopicBase +mTMParentTopic,
		      TopicBase + TopicMenu,
		      "Parent Topic",
		      1,
		      "To do",
		      FALSE,
		      'L')

#endif

   TtaNewTextForm (TopicBase + mTMParentTopic,
		   TopicBase + TopicMenu,
		   "Parent Topic:",
		   30,
		   1,
		   TRUE);

   TtaNewTextForm (TopicBase + mTMTopicURL,
		   TopicBase + TopicMenu,
		   "Topic URL:",
		   30,
		   1,
		   TRUE);

   TtaNewTextForm (TopicBase + mTMTitle,
		   TopicBase + TopicMenu,
		   "Topic Title:",
		   30,
		   1,
		   TRUE);

   TtaNewTextForm (TopicBase + mTMAuthor,
		   TopicBase + TopicMenu,
		   "Author:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (TopicBase + mTMCreated,
		   TopicBase + TopicMenu,
		   "Created:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (TopicBase + mTMModified,
		   TopicBase + TopicMenu,
		   "Last Modified:",
		   30,
		   1,
		   TRUE);
   TtaNewTextForm (TopicBase + mTMDescription,
		   TopicBase + TopicMenu,
		   "Description:",
		   30,
		   5,
		   TRUE);
#endif /* !_WINDOWS */

#if 0
   /* load and display the current values */
   GetAnnotConf ();
#endif

   InitTopicMenu (doc, bookmark);
#ifndef _WINDOWS
   RefreshTopicMenu ();
  /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (TopicBase + TopicMenu, TRUE);
#else
  if (!AnnotHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTMENU), NULL, (DLGPROC) WIN_AnnotDlgProc);
  else
     SetFocus (AnnotHwnd);
#endif /* !_WINDOWS */
}


