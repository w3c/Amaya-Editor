#ifndef BOOKMARKS_H
#define BOOKMARKS_H
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2003.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * Author: J. Kahan (W3C/ERCIM)
 *
 */

#define THOT_EXPORT extern

/* RDF Property names */
#define BOOKMARK_NS "http://www.w3.org/2002/01/bookmark"
#define BM_BOOKMARK "Bookmark"
#define BMNS_BOOKMARK BOOKMARK_NS"#"BM_BOOKMARK
/* @@ JK: Change BOOKMARKS fo recalls everyhwere */
#define BM_BOOKMARKS "recalls"
#define BMNS_BOOKMARKS BOOKMARK_NS"#"BM_BOOKMARKS
#define BM_TOPIC "Topic"
#define BMNS_TOPIC BOOKMARK_NS"#"BM_TOPIC
#define BM_SUBTOPICOF "subTopicOf"
#define BMNS_SUBTOPICOF BOOKMARK_NS"#"BM_SUBTOPICOF
#define BM_HASTOPIC "hasTopic"
#define BMNS_HASTOPIC BOOKMARK_NS"#"BM_HASTOPIC
#endif /* BOOKMARKS_H */

/* The local bookmarks file */
#define LOCAL_BOOKMARKS_FILE "bookmarks.rdf"

/* The default home topic name */
#define HOME_TOPIC_ANCHOR "#MyHomeTopic"

/* the default home topic title */
#define HOME_TOPIC_TITLE "My Home Topic"

typedef struct _Bookmark
{
  ThotBool  isUpdate;   /* says if we're creating or updating an item */
  ThotBool  isTopic;    /* says if this item is a bookmark or a topic */
  char *parent_url;     /* the parent topic */
  List *parent_url_list;
  char *self_url;       /* the id that talks about this item */
  char *bookmarks;      /* for bookmarks, what we're bookmarking */
  char *title;      
  char *author;
  char *created;
  char *modified;
  char *description;
  char *context;   /* not used yet, but XPointer like */
} Bookmark, *BookmarkP;

typedef struct _BM_dyn_buffer {
  char *buffer;
  unsigned int lgbuffer;
} BM_dyn_buffer;


typedef struct _dynBookmark
{
  BM_dyn_buffer *parent_url;     /* the parent topic */
  BM_dyn_buffer *self_url;       /* the id that talks about this item */
  BM_dyn_buffer *bookmarks;      /* for bookmarks, what we're bookmarking */
  BM_dyn_buffer *title;      
  BM_dyn_buffer *author;
  BM_dyn_buffer *description;
  int ref;                       /* context reference */
  Document doc;                  /* the document from which this widget was 
				    called */
} dynBookmark, *dynBookmarkP;




