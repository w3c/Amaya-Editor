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
static BookmarkP aTopic;
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
    }

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
    {
      tmp = TtaConvertMbsToByte (initial->title, ISO_8859_1);
      strcpy (me->title, tmp);
      TtaFreeMemory (tmp);
    }
  else
    me->title[0] = EOS;
  me->author = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->author)
    {
      tmp = TtaConvertMbsToByte (initial->author, ISO_8859_1);
      strcpy (me->author, tmp);
      TtaFreeMemory (tmp);
    }
  else
    me->author[0] = EOS;
  me->description = TtaGetMemory (MAX_LENGTH + 1);
  if (initial && initial->description)
    {
      tmp = TtaConvertMbsToByte (initial->description, ISO_8859_1);
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
  char *ptr2;

  aBookmark = Bookmark_new_init (bookmark, FALSE);

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

	  ptr2 = TtaGetMemory (strlen (ptr) 
			       + strlen (DocumentMeta[doc]->form_data)
			       + 2);

	  sprintf (ptr2, "%s?%s", ptr, DocumentMeta[doc]->form_data);
	  if (ptr != DocumentURLs[doc])
	    TtaFreeMemory (ptr);
	  ptr = ptr2;
	}
      strcpy (aBookmark->bookmarks, ptr);

      if (ptr != DocumentURLs[doc])
	TtaFreeMemory (ptr);

      ptr = ANNOT_GetHTMLTitle (doc);
      ptr2 = TtaConvertMbsToByte (ptr, ISO_8859_1);
      strcpy (aBookmark->title, ptr2);
      TtaFreeMemory (ptr);
      TtaFreeMemory (ptr2);
      ptr = GetAnnotUser ();
      ptr2 = TtaConvertMbsToByte (ptr, ISO_8859_1);
      strcpy (aBookmark->author, ptr2);
      TtaFreeMemory (ptr2);
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
  TtaSetTextForm (BookmarkBase + mBMTitle, aBookmark->title);
  TtaSetTextForm (BookmarkBase + mBMBookmarks, aBookmark->bookmarks);
  TtaSetTextForm (BookmarkBase + mBMAuthor, aBookmark->author);
  TtaSetTextForm (BookmarkBase + mBMCreated, aBookmark->created);
  TtaSetTextForm (BookmarkBase + mBMModified, aBookmark->modified);
  TtaSetTextForm (BookmarkBase + mBMDescription, aBookmark->description);
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
      
      /* @@ JK: just for debugging */
      if (state)
	printf ("Selected URL %s\n", url);
      else
	printf ("Deselected URL %s\n", url);
      TtaFreeMemory (url);
    }
}


/*----------------------------------------------------------------------
  BookmarkMenuCallbackDialog
  callback of the Bookmark menu
  ----------------------------------------------------------------------*/
static void BookmarkMenuCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;
  ThotBool            result;
  List                *parent_url_list;

  if (ref == -1)
    {
      /* removes the bookmark menu */
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
	      aBookmark = NULL;
	      TtaCloseDocument (BTopicTree);
	      BTopicTree = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create bookmark */
	      ControlURIs (aBookmark);
	      BM_dumpTopicTreeSelections (BTopicTree, &parent_url_list);
	      if (parent_url_list)
		{
		  if (aBookmark->parent_url_list)
		    {
		      List *tmp;
		      tmp = aBookmark->parent_url_list;
		      List_delAll (&tmp, (void *) TtaFreeMemory);
		    }
		  aBookmark->parent_url_list = parent_url_list;
		  if (aBookmark->isUpdate)
		    result = BM_updateItem (aBookmark, FALSE);
		  else
		    result = BM_addBookmark (aBookmark);
		  if (result)
		    {
		      /* if successful, close the dialog */
		      Bookmark_free (aBookmark);
		      aBookmark = NULL;
		      TtaCloseDocument (BTopicTree);
		      BTopicTree = 0;
		      TtaDestroyDialogue (ref);
		      /* refresh the view */
		      BM_refreshBookmarkView ();
		    }
		}
	      break;
	    case 2: /* new topic */
	      BM_TopicMenu (1, 1, NULL);
	      break;

	    default:
	      break;
	    }
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
   ThotWidget tree;

   if (BookmarkBase == 0)
     BookmarkBase = TtaSetCallback (BookmarkMenuCallbackDialog, MAX_BOOKMARKMENU_DLG);
   
   if (aBookmark)
     {
       Bookmark_free (aBookmark);
       aBookmark = NULL;
     }

   /* dump the topics as a thot three */
   if (BTopicTree != 0)
     TtaCloseDocument (BTopicTree);
   BTopicTree = BM_GetTopicTree ();
   
   /* select the topics in the tree that correspond to those in the bookmark */
   BM_topicsPreSelect (BTopicTree, bookmark);

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i = strlen (&s[i]) + 1;
   strcpy (&s[i], "New Topic");

   TtaNewSheet (BookmarkBase + BookmarkMenu, 
		TtaGetViewFrame (doc, view),
		"Bookmark Properties",
		2, s, FALSE, 9, 'L', D_DONE);


   tree = TtaNewTreeForm (BookmarkBase + mBMTopicTree,
			  BookmarkBase + BookmarkMenu,
			  "Topic hierarchy:",
			  TRUE);
   
   if (tree)
     BM_InitTreeWidget (tree, BTopicTree, (void *) BookmarkMenuSelect_cbf);

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

   /* create the new bookmark structure */
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
  char *annotUser;
  char *tmp;

  aTopic = Bookmark_new_init (bookmark, TRUE);
  aTopic->isTopic = TRUE;

  if (!bookmark)
    {
      annotUser = GetAnnotUser ();
      tmp = TtaConvertMbsToByte (annotUser, ISO_8859_1);
      strcpy (aTopic->author, tmp);
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
  /* set the menu entries to the current values */
  TtaSetTextForm (TopicBase + mTMParentTopic, aTopic->parent_url);
  TtaSetTextForm (TopicBase + mTMTitle, aTopic->title);
  TtaSetTextForm (TopicBase + mTMAuthor, aTopic->author);
  TtaSetTextForm (TopicBase + mTMCreated, aTopic->created);
  TtaSetTextForm (TopicBase + mTMModified, aTopic->modified);
  TtaSetTextForm (TopicBase + mTMDescription, aTopic->description);
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
      if (state)
	printf ("Selected URL %s\n", url);
      else
	printf ("Deselected URL %s\n", url);
      strcpy (aTopic->parent_url, url);
      /* @@ JK: just for debugging */
      TtaSetTextForm (TopicBase + mTMParentTopic, aTopic->parent_url);
      TtaFreeMemory (url);
    }
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
      /* removes the topic menu */
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
	      Bookmark_free (aTopic);
	      aTopic = NULL;
	      TtaCloseDocument (TTopicTree);
	      TTopicTree = 0;
	      TtaDestroyDialogue (ref);
	      break;
	    case 1: /* create topic */
	      ControlURIs (aTopic);
	      if (aTopic->isUpdate)
		result = BM_updateItem (aTopic, TRUE);
	      else
		result = BM_addTopic (aTopic, TRUE);
	      if (result)
		{
		  /* if successful, close the dialog */
		  Bookmark_free (aTopic);
		  aTopic = NULL;
		  TtaCloseDocument (TTopicTree);
		  TTopicTree = 0;
		  TtaDestroyDialogue (ref);
		  /* refresh the topic view */
		  BM_refreshBookmarkView ();
		  /* and the open widgets */
		  BM_RefreshTopicTree ();
		}
	      break;

	    default:
	      break;
	    }
	  break;

	case mTMParentTopic:
	  if (data)
	    strcpy (aTopic->parent_url, data);
	  else
	    aTopic->parent_url[0] = EOS;
	  break;

	case mTMTitle:
	  if (data)
	    strcpy (aTopic->title, data);
	  else
	    aTopic->title[0] = EOS;
	  break;

	case mTMAuthor:
	  if (data)
	    strcpy (aTopic->author, data);
	  else
	    aTopic->author[0] = EOS;
	  break;

	case mTMDescription:
	  if (data)
	    strcpy (aTopic->description, data);
	  else
	    aTopic->description[0] = EOS;
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
   ThotWidget tree;

   if (TopicBase == 0)
     TopicBase = TtaSetCallback (TopicMenuCallbackDialog, MAX_TOPICMENU_DLG);
  
   if (aTopic)
     {
       Bookmark_free (aTopic);
       aTopic = NULL;
     }
   /* dump the topics as a thot three */
   if (TTopicTree != 0)
     TtaCloseDocument (TTopicTree);
   TTopicTree = BM_GetTopicTree ();

   /* remove the child entries. We cannot move a topic to its children */
   BM_topicsPrune (TTopicTree, bookmark);

   /* select the topics in the tree that correspond to those in the bookmark */
   BM_topicsPreSelect (TTopicTree, bookmark);

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));

   TtaNewSheet (TopicBase + TopicMenu, 
		TtaGetViewFrame (doc, view),
		"Topic Properties",
		1, s, FALSE, 9, 'L', D_DONE);

   tree = TtaNewTreeForm (TopicBase + mTMTopicTree,
		   TopicBase + TopicMenu,
		   "Topic hierarchy:",
		   FALSE);

   if (tree)
     BM_InitTreeWidget (tree, TTopicTree, (void *) TopicMenuSelect_cbf);

   TtaNewTextForm (TopicBase + mTMParentTopic,
		   TopicBase + TopicMenu,
		   "Parent Topic:",
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

   /* make the new topic */
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


/*----------------------------------------------------------------------
  TopicURLCallbackDialog
  callback of the Bookmark menu
  ----------------------------------------------------------------------*/
static void TopicURLCallbackDialog (int ref, int typedata, char *data)
{
  int                 val;
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
      val = (int) data;
      switch (ref - TopicURLBase)
	{
	case BookmarkMenu:
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
#ifndef _WINDOWS
	  TtaSetTextForm (TopicURLBase + mTUMURL, TopicDirectory);
#endif /* !_WINDOWS */
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
	    /* set path on current directory */
	    getcwd (TopicDirectory, MAX_LENGTH);
	  /* construct the full name of the topic */
	  strcpy (LastURLTopic, TopicDirectory);
	  val = strlen (LastURLTopic) - 1;
	  if (LastURLTopic[val] != DIR_SEP)
	    strcat (LastURLTopic, DIR_STR);
	  strcat (LastURLTopic, data);
#ifndef _WINDOWS
	  TtaSetTextForm (TopicURLBase + mTUMURL, LastURLTopic);
#endif /* !_WINDOWS */
	  break;

    case mTUMFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (TopicFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (TopicURLBase + mTUMFilter, TopicFilter);
#endif /* !_WINDOWS */
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
   /* set path on current directory */
   getcwd (TopicDirectory, MAX_LENGTH);
   LastURLTopic[0] = EOS;
   strcpy (TopicFilter, "*.rdf");
}

/*----------------------------------------------------------------------
  GetTopicURL Gets the URL of a topic hierarchy to download
  ----------------------------------------------------------------------*/
char *GetTopicURL (Document document, View view)
{
#ifndef _WINDOWS
   char               s[MAX_LENGTH];
   int                 i;

   if (TopicURLBase == 0)
     TopicURLBase = TtaSetCallback (TopicURLCallbackDialog, MAX_TOPICURLMENU_DLG);

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

   TtaShowDialogue (TopicURLBase + TopicURLMenu, FALSE);
   TtaWaitShowDialogue ();

#endif /* _WINDOWS */
   return (LastURLTopic);

}

/*----------------------------------------------------------------------
  BM_RefreshTopicTree
  Redisplays the topic tree in the topic and bookmark widgets, if they
  are open.
  ----------------------------------------------------------------------*/
void BM_RefreshTopicTree ()
{
  ThotWidget tree;

  if (aBookmark)
    {
      tree = TtaCatWidget (BookmarkBase + mBMTopicTree);
      tree = TtaClearTree (tree);
      if (BTopicTree)
	TtaCloseDocument (BTopicTree);
      BTopicTree = BM_GetTopicTree ();
      BM_InitTreeWidget (tree, BTopicTree, (void *) BookmarkMenuSelect_cbf);
    }
}
